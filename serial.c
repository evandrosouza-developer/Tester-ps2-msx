#include "serial.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

// See the inspiring file:
// https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/stm32-h103/usart_irq_printf/usart_irq_printf.c

#define USART_PORT USART2

#define X_ON	17
#define X_OFF 19
#define X_OFF_TRIGGER (3*SERIAL_RING_BUFFER_SIZE/4)
#define X_ON_TRIGGER (SERIAL_RING_BUFFER_SIZE/2)

volatile bool enable_xon_xoff = true, xon_condition = true, xoff_condition = false, xonoff_sendnow = false;

struct ring tx_ring;
struct ring rx_ring;

uint8_t tx_ring_buffer[SERIAL_RING_BUFFER_SIZE];
uint8_t rx_ring_buffer[SERIAL_RING_BUFFER_SIZE];

//Prototypes:
void ring_init(struct ring *ring, uint8_t *buf);
int _write(int, char*, int);


void ring_init(struct ring *ring, uint8_t *buf)
{
	ring->data = buf;
	ring->put_ptr = 0;
	ring->get_ptr = 0;
}


//Used as an internal function.
//It is used to put a char in the ring buffer, both TX and RX.
//It returns number of chars are in the buffer of 0xFFFF when there was no room to add this char.
uint16_t ring_put_ch(struct ring *ring, uint8_t ch)
{
	uint16_t i, i_next;
	i = ring->put_ptr;				//i is the original position
	i_next = i + 1;						//i_next is the next position of i
	i_next = i_next & (uint16_t)(SERIAL_RING_BUFFER_SIZE - 1); //if(i_next >= (uint16_t)SERIAL_RING_BUFFER_SIZE) i_next = 0;
	if(i_next != ring->get_ptr)
	{
		ring->data[i] = ch;			//saves in the put_ptr position 
		ring->put_ptr = i_next;	//now put_ptr points to the next position of i
		//Optimizing calculations inside the interrupt => The general formula is:
		//CharsInBuffer = (RING_BUFFER_SIZE - ring.get_ptr + ring.put_ptr) % RING_BUFFER_SIZE;
		//but SERIAL_RING_BUFFER_SIZE is a power of two, so the rest of the division is computed zeroing
		//the higher bits of the summed buffer lenght, so in the case of 256 (2**8), you have to keep only
		//the lowest 8 bits: (SERIAL_RING_BUFFER_SIZE - 1).
		return (uint16_t)(SERIAL_RING_BUFFER_SIZE - ring->get_ptr + i_next) & (uint16_t)(SERIAL_RING_BUFFER_SIZE - 1);
	}
	else
	{
		//No room for more (Buffer full)
		return 0xFFFF;
	}
}

//Used as an internal function.
//It is used to put a char in the ring TX buffer, as it will initiate TX if the first one is put on buffer.
//It returns number of chars are in the buffer of 0xFFFF when there was no room to add this char.
static uint16_t ring_tx_put_ch(uint8_t ch)
{
	uint16_t result;

	result = ring_put_ch(&tx_ring, ch);
	if (result == 1)
	{
		// Enable Transmit request
		USART_CR1(USART_PORT) |= USART_CR1_TXEIE;
	}
	return result;
}


//Used as an internal function.
// Used to write more than one char on tx_ring and initiate a transmission
int _write(int file, char *ptr, int len)
{
	// If the target file isn't stdout/stderr, then return an error
	// since we don't _actually_ support file handles
	if (file != STDOUT_FILENO && file != STDERR_FILENO) {
		// Set the errno code (requires errno.h)
		errno = EIO;
		return -1;
	}
	int i;
	for (i = 0; i < len; i++)
	{
		// If we get a newline character, also be sure to send the carriage
		// return character first, otherwise the serial console may not
		// actually return to the left.
		if (ptr[i] == '\n') 
			while (ring_tx_put_ch('\r') == 0xFFFF)  //usart_send_blocking(USART_PORT, '\r');
				__asm("nop");
		// Write the character to send to the USART transmit buffer, and block
		// until it has been sent.
		while (ring_tx_put_ch(ptr[i]) == 0xFFFF)	//usart_send_blocking(USART_PORT, ptr[i]);
	__asm("nop");
	}
	// Return the number of bytes we sent
	return i;
}


//Used as an internal function.
//Returns true if there is a char available to read in the ring (both TX and RX) or false if not.
static uint16_t ring_avail_get_ch(struct ring *ring)
{
	uint16_t output;
	output = (SERIAL_RING_BUFFER_SIZE - ring->get_ptr + ring->put_ptr) & (SERIAL_RING_BUFFER_SIZE - 1);
	return output;
}


//Used as an internal function.
//It returns char when it is available or -1 when no one is available
//Used on both TX and RX buffers.
int16_t ring_get_ch(struct ring *ring, uint16_t *qty_in_buffer)
{
	uint16_t i = ring->get_ptr;
	if(i == ring->put_ptr)
		//No char in buffer
		return -1;
	int16_t result = (int16_t)ring->data[i];
	*qty_in_buffer = (SERIAL_RING_BUFFER_SIZE - i + ring->put_ptr) & (SERIAL_RING_BUFFER_SIZE - 1);
	i++;
	ring->get_ptr = i & (uint16_t)(SERIAL_RING_BUFFER_SIZE - 1); //if(i >= (uint16_t)SERIAL_RING_BUFFER_SIZE)	i = 0;
	return result;
}



//Used as an internal function.
//Implemented X_ON/X_OFF protocol
//It returns char when it is available or -1 when no one is available
//Used only on RX buffer.
static int16_t ring_rx_get_ch(uint16_t *qty_in_buffer)
{
	int16_t ch;

	ch = ring_get_ch(&rx_ring, qty_in_buffer);

	if (enable_xon_xoff)
	{
		if (*qty_in_buffer >= (uint16_t)X_OFF_TRIGGER)
		{
			xon_condition = false;
			if (!xoff_condition)										//To send X_OFF only once
			{
				xoff_condition = true;
				// Enable Transmit request
				xonoff_sendnow = true;
				USART_CR1(USART_PORT) |= USART_CR1_TXEIE; //Force Enable transmission request
			}
		}
		else if (*qty_in_buffer <= (uint16_t)X_ON_TRIGGER)
		{
			xoff_condition = false;
			if (!xon_condition)		//To send X_ON only once
			{
				xon_condition = true;
				// Enable Transmit request
				xonoff_sendnow = true;
				USART_CR1(USART_PORT) |= USART_CR1_TXEIE; //Force Enable transmission request
			}  //if (!xon_condition)
		} //else if (*qty_in_buffer <= (uint16_t)X_ON_TRIGGER)
	} //if (enable_xon_xoff)
	return ch;
}


//Ready to be used from outside of this module.
// If there is an available char in USART_PORT RX ring, it returns true.
uint16_t serial_available_get_char(void)
{
	return (ring_avail_get_ch(&rx_ring));
}


//Ready to be used from outside of this module.
// If there is an available char in serial, it returns with an uint8_t.
//You MUST use the above function "serial_available_get_char" BEFORE this one,
//in order to verify a valid available reading by this function.
uint8_t serial_get_char(void)
{
	uint16_t bin; //information to be discarded
	return (uint8_t)ring_rx_get_ch(&bin);
}


//Ready to be used outside this module.
// Put a char (uint8_t) on serial buffer.
// It returns true if there was room to put this on USART_PORT TX buffer.
bool serial_put_char(uint8_t ch)
{
	/* Put char in tx_ring. */
	bool result = ( ring_tx_put_ch(ch) != 0xFFFF ) ? true : false;
	return result;
}


//Ready to be used outside this module.
// Put an ASCIIZ (uint8_t) string on serial buffer.
// No return  and no blocking function if there is enough space on USART_PORT TX buffer, otherwise,
// wait until buffer is filled.
void usart_send_string(uint8_t *string)
{
	uint16_t iter = 0;
	do
	{	//usart_send_blocking(usart, string[iter++]);
		while (!serial_put_char(string[iter]))
		{
			__asm("nop");
		}
		iter++;	//points to next char on the string
	}	while(string[iter] != 0 && iter < 128);
}


//Ready to be used outside this module.
// Convert a two byte string pointed by i into a binary byte. 
// It returns and no blocking function if there is enough space on USART_PORT TX buffer, otherwise,
uint8_t conv_2a_hex_to_uint8(uint8_t *instring, int16_t i)
{
	uint8_t binuint8, ch;

	ch = instring[i];// & 0x5F; //to capital letter
	binuint8 = (ch > ('A'-1) && ch < ('F'+1)) ?
						 (ch-('A'-10)) : (ch-'0'); //55 = "A"0x41-10; 48="0"0x30
	binuint8 <<= 4;
	ch = instring[i+1];// & 0x5F; //to capital letter
	binuint8 += (ch > ('A'-1) && ch < ('F'+1)) ?
							(ch-('A'-10)) : (ch-'0'); //55 = "A"0x41-10; 48="0"0x30
	return binuint8;
}


//Ready to be used outside this module.
// Convert a word (32 bit binary) to into a 8 char string. 
void conv_uint32_to_8a_hex(uint32_t value, uint8_t *outstring)
{
	uint8_t iter;

	/*end of string*/
	outstring += 8;
	*(outstring--) = 0;

	for(iter=0; iter<8; iter++)
	{
		*(outstring--) = (((value&0xf) > 0x9) ? (0x40 + ((value&0xf) - 0x9)) : (0x30 | (value&0xf)));
		value >>= 4;
	}
}


//Ready to be used outside this module.
// Convert a half-word (16 bit binary) to into a 4 char string. 
void conv_uint16_to_4a_hex(uint16_t value, uint8_t *outstring)
{
	uint8_t iter;

	/*end of string*/
	outstring += 4;
	*(outstring--) = 0;

	for(iter=0; iter<4; iter++)
	{
		*(outstring--) = (((value&0xf) > 0x9) ? (0x40 + ((value&0xf) - 0x9)) : (0x30 | (value&0xf)));
		value >>= 4;
	}
}


//Ready to be used outside this module.
// Convert a word (32 bit binary) to into a 10 char string. 
void conv_uint32_to_dec(uint32_t value, uint8_t *outstring)
{
	uint8_t iter, pos;
	const uint32_t power10[10] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

	bool insertzero = false;
	for(iter=0; iter<=10; iter++)	//Fill outstring with \0
		(outstring[iter]) = 0;

	if(value == 0)
		outstring[0] = '0';
	else
	{
		pos = 0;
		for(iter=0; iter<10; iter++)
		{
			if ((value >= power10[iter]) || insertzero)
			{
				outstring[pos] = '0';
				while (value >= power10[iter])
				{
					value -= power10[iter];
					outstring[pos] += 1;
					insertzero = true;
				}
			}	//if (value > power10[iter])
			if (insertzero)
				pos++;
		}	//for(iter=0; iter<5; iter++)
	}
}


//Ready to be used outside this module.
// Convert a byte (8 bit binary) to into a 2 char string. 
void conv_uint8_to_2a_hex(uint8_t value, uint8_t *outstring)
{
	uint8_t iter;

	/*end of string*/
	outstring += 2;
	*(outstring--) = 0;

	for(iter=0; iter<2; iter++)
	{
		*(outstring--) = (((value&0xf) > 0x9) ? (0x40 + ((value&0xf) - 0x9)) : (0x30 | (value&0xf)));
		value >>= 4;
	}
}



//Ready to be used outside this module.
// Setup serial used in main.
void serial_setup(void)
{
#if	USART_PORT == USART1
	// Enable clocks for GPIO port A (for GPIO_USART1_TX in main) and USART_PORT.
	rcc_periph_clock_enable(RCC_USART1);

	// Enable the USART_PORT interrupt.
	nvic_enable_irq(NVIC_USART1_IRQ);

	// Setup GPIO pin GPIO_USART1_RE_TX on GPIO port A for transmit.
	gpio_set_mode(GPIO_BANK_USART1_TX, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIO_BANK_USART1_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
#endif

#if	USART_PORT == USART2
	// Enable clocks for GPIO port A (for GPIO_USART1_TX in main) and USART_PORT.
	rcc_periph_clock_enable(RCC_USART2);

	// Enable the USART_PORT interrupt.
	nvic_enable_irq(NVIC_USART2_IRQ);

	// Setup GPIO pin GPIO_USART2_RE_TX on GPIO port A for transmit.
	gpio_set_mode(GPIO_BANK_USART2_TX, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
	gpio_set_mode(GPIO_BANK_USART2_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);
#endif

	// Initialize input and output ring buffers.
	ring_init(&tx_ring, tx_ring_buffer);
	ring_init(&rx_ring, rx_ring_buffer);

	// Setup UART parameters.
	usart_set_baudrate(USART_PORT, 115200);
	usart_set_databits(USART_PORT, 8);
	usart_set_stopbits(USART_PORT, USART_STOPBITS_1);
	usart_set_parity(USART_PORT, USART_PARITY_NONE);
	usart_set_flow_control(USART_PORT, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART_PORT, USART_MODE_TX_RX);

	/* Enable USART_PORT Receive interrupt. */
	USART_CR1(USART_PORT) |= USART_CR1_RXNEIE;
	
	// Enable the USART.
	usart_enable(USART_PORT);

	// Finally init X_ON/X_OFF flags.
	xon_condition = true;
	xoff_condition = false;
	xonoff_sendnow = false;
}


/*************************************************************************************************/
/*************************************************************************************************/
/******************************************* ISR's ***********************************************/
/*************************************************************************************************/
/*************************************************************************************************/
//XON/XOFF flux control protocol is implemented on RX
#if	USART_PORT == USART1
void usart1_isr(void)
#endif
#if	USART_PORT == USART2
void usart2_isr(void)
#endif
{
	// Check if we were called because of RXNE.
	if(((USART_CR1(USART_PORT)&USART_CR1_RXNEIE)!=0) && ((USART_SR(USART_PORT)&USART_SR_RXNE)!=0))
	{
		// Retrieve the data from the peripheral and put in rx_ring.
		ring_put_ch(&rx_ring, (uint8_t)usart_recv(USART_PORT));
	}

	// Check if we were called because of TXE.
	if(((USART_CR1(USART_PORT) & USART_CR1_TXEIE)!=0) && ((USART_SR(USART_PORT) & USART_SR_TXE)!=0))
	{
		int16_t data;
		uint16_t qty_in_buffer;

		if (xonoff_sendnow)
		{
			if(xoff_condition)
				data = X_OFF;
			if(xon_condition)
				data = X_ON;
			xonoff_sendnow = false;
		}
		else
			data = ring_get_ch(&tx_ring, &qty_in_buffer);

		if (data == -1) 
		{ // Disable the TXE interrupt: It's no longer needed.
			USART_CR1(USART_PORT) &= ~USART_CR1_TXEIE;
		}
		else 
		{ //  Put data into the transmit register.
			usart_send(USART_PORT, (uint16_t) data);
		}
	}
}
