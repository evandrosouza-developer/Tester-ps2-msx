#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>

#include "t_ps2handl.h"
#include "t_msxmap.h"
#include "t_port_def.h"
#include "serial.h"

//Use Tab width=2

//Variáveis globais: Visíveis por todo o contexto do programa
extern uint32_t systicks;											//Declared on sys_timer.cpp
extern uint8_t init_scancount, end_scancount;	//Declared on t_sys_timer.cpp
extern uint8_t y_scan;												//Declared on t_sys_timer.cpp
extern volatile uint64_t TIM2_Update_Cnt;			//Declarated on t_hr_timer.c
extern volatile uint64_t u64_TIM2_Cnt;				//Declarated on t_hr_timer.c
volatile uint8_t msx_X;

volatile uint32_t previous_y_systick[ 16 ] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//MSX Keyboard - Used to signalize status change in MSX matrix
uint8_t msx_matrix[ 16 ] =  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
														 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	//The index is used to store Y

uint8_t mountISRstring[SERIAL_RING_BUFFER_SIZE];
struct ring isr_string_ring;

//Prototype
void isr_string_concat(uint8_t*, struct ring *);
void ring_init(struct ring *, uint8_t*);


void ring_init(struct ring *ring, uint8_t *buf)
{
	ring->data = buf;
	ring->put_ptr = 0;
	ring->get_ptr = 0;
}


void msxmap::msx_interface_setup(void)
{
	//Not the STM32 default: Pull up;
	gpio_set(X7_port,
	X7_pin_id | X6_pin_id | X5_pin_id | X4_pin_id | X3_pin_id | X2_pin_id | X1_pin_id | X0_pin_id);

	//Init output port B15:8
	gpio_set_mode(X7_port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, 
	X7_pin_id | X6_pin_id | X5_pin_id | X4_pin_id | X3_pin_id | X2_pin_id | X1_pin_id | X0_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC3 MSX 8255 Pin 17)
	gpio_set(Y3_port, Y3_pin_id); //pull up resistor
	gpio_set_mode(Y3_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Y3_pin_id); // PC3 (MSX 8255 Pin 17)
	gpio_port_config_lock(Y3_port, Y3_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC2 MSX 8255 Pin 16)
	gpio_set(Y2_port, Y2_pin_id); //pull up resistor
	gpio_set_mode(Y2_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Y2_pin_id); // PC2 (MSX 8255 Pin 16)
	gpio_port_config_lock(Y2_port, Y2_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC1 MSX 8255 Pin 15)
	gpio_set(Y1_port, Y1_pin_id); //pull up resistor
	gpio_set_mode(Y1_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Y1_pin_id); // PC1 (MSX 8255 Pin 15)
	gpio_port_config_lock(Y1_port, Y1_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC0 MSX 8255 Pin 14)
	gpio_set(Y0_port, Y0_pin_id); //pull up resistor
	gpio_set_mode(Y0_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Y0_pin_id); // PC0 (MSX 8255 Pin 14)
	gpio_port_config_lock(Y0_port, Y0_pin_id);

	// GPIO pin for Oscilloscope sync
	gpio_set(Y_port, Y_pin_id); //pull up resistor
	gpio_set_mode(Y_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Y_pin_id); // PC0 (MSX 8255 Pin 14)
	gpio_port_config_lock(Y_port, Y_pin_id);

	// GPIO pins for CAPS & KANA
	gpio_set(CAPS_port, CAPS_pin_id); //pull up resistor
	gpio_set_mode(CAPS_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, CAPS_pin_id);
	gpio_port_config_lock(CAPS_port, CAPS_pin_id);

	gpio_set_mode(KANA_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, KANA_pin_id);
	gpio_port_config_lock(KANA_port, KANA_pin_id);

	// Initialize input and output ring buffers.
	ring_init(&isr_string_ring, mountISRstring);
}


void msxmap::general_debug_setup(void)
{
	gpio_set_mode(SYSTICK_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, SYSTICK_pin_id);
	gpio_set(SYSTICK_port, SYSTICK_pin_id); //Default condition is "1"

	gpio_set_mode(Xint_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Xint_pin_id); // PC2 e 3 (MSX 8255 Pin 17)
	gpio_set(Xint_port, Xint_pin_id); //Default condition is "1"
	
	gpio_set_mode(INT_TIM2_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, TIM2UIF_pin_id); // PC2 e 3 (MSX 8255 Pin 17)
	gpio_set(INT_TIM2_port, TIM2UIF_pin_id); //Default condition is "1"
	
}


// Concat an ASCIIZ (uint8_t) string on ISR String Mounting buffer.
void isr_string_concat(uint8_t *string_org, struct ring *str_mount_buff)
{
	uint8_t ch;

	uint16_t i = 0;
	do
	{
		ch = string_org[i];
		if (ch == 0) //quit do-while
			break;
		ring_put_ch(str_mount_buff, ch);
		i++;	//points to next char on the stringorg
	}	while(i < SERIAL_RING_BUFFER_SIZE);
}


//This routine will be called from delay_usec (from t_sys_timer / t_hr_timer)
void portXread(void)
{
	uint8_t mountstring[6];					//Used in usart_send_string()

	//To be measured the real time from writing to reading, by putting an oscilloscope at pin A1
	GPIO_BSRR(Xint_port) = Xint_pin_id; //Back to default condition ("1")

	// Read the MSX keyboard X answer through GPIO pins B15:8
	msx_X = (gpio_port_read(GPIOB)>>8) & 0xFF; //Read bits B15:8
	
	if (msx_X != msx_matrix[y_scan])
	{
		//Read the result of this reading and mount it to a circular buffer string
		msx_matrix[y_scan] = msx_X;
		//Print the changes through filling buffer that will be transfered to serial in main
		//Print y_scan, msx_X and readtimer
		isr_string_concat((uint8_t*)"Y", &isr_string_ring);
		conv_uint8_to_2a_hex(y_scan, &mountstring[0]);
		isr_string_concat(&mountstring[1], &isr_string_ring);
		isr_string_concat((uint8_t*)" X", &isr_string_ring);
		conv_uint8_to_2a_hex(msx_X, &mountstring[0]);
		isr_string_concat((uint8_t*)&mountstring[0], &isr_string_ring);
		isr_string_concat((uint8_t*)"\r\n> ", &isr_string_ring);
	}
	//Update here to next valid scan
	y_scan++;
	if (y_scan > end_scancount)
	{
		y_scan = init_scancount;
	}
}
