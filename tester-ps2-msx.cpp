/*
 * This file was part of the libopencm3, but it was edited to implement a PS/2 to MSX keyboard project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

//Use Tab width=2

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "t_sys_timer.h"
#include "serial.h"
#include "t_hr_timer.h"
#include "t_msxmap.h"
#include "t_port_def.h"

//Variáveis globais
uint32_t scan_pointer;
uint32_t delay_to_read_x_scan;
extern uint32_t systicks;													//Declared on t_sys_timer.cpp
extern bool ticks_keys, wait_flag, single_sweep;	//Declared on t_sys_timer.cpp
extern bool single_step;													//Declared on t_sys_timer.cpp
extern uint8_t init_scancount, end_scancount;			//Declared on t_sys_timer.cpp
extern uint8_t y_scan;														//Declared on t_sys_timer.cpp
extern volatile uint8_t caps_line, kana_line;			//Declared on t_sys_timer.cpp
extern bool update_ps2_leds, ps2numlockstate;			//Declared on t_msxmap.cpp
extern bool compatible_database;									//Declared on t_msxmap.cpp
extern uint8_t mountISRstring[SERIAL_RING_BUFFER_SIZE];		//Declared on t_msxmap.cpp
extern struct ring isr_string_ring;
uint16_t bin;


//Scan speed selection
const uint8_t SPEED_SELEC[SCAN_POINTER_SIZE][8]= {"1.00000", "2.00000", "4.00000", "8.00000", "16.0000", "32.0000",
																									"64.0000", "128.000", "256.000", "512.000", "1024.01", "2048.25",
																									"4096.50", "8196.72", "16423.4", "32491.0", "60000.0", "120000."};


const uint8_t TIME_TO_READ_X[DELAY_TO_READ_SIZE][5]= {"2.25", "2.40", "2.65", "2.90", "3.15", "3.40", "3.65",
																											"3.90", "4.15", "4.40", "4.65", "4.90", "5.15", "5.40"};

int main(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();	// Use this for "blue pill"

	rcc_periph_clock_enable(RCC_AFIO); //Have to clock AFIO to use PA15 and PB4 freed by gpio remap below

	// Bits 9:8 TIM2_REMAP[1:0]: TIM2 remapping - 01: Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)
	gpio_primary_remap(AFIO_MAPR_TIM2_REMAP_PARTIAL_REMAP1, 0);

	// Full Serial Wire JTAG capability without JNTRST
	//gpio_primary_remap(AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST, 0);

	// Disable JTAG, enable SWD. This frees up GPIO PA15 (JTDI), PB3 (JTDO / TRACESWO) and PB4 (NJTRST)
	// GPIO PA13 (SWDIO) and PA14 (SWCLK) are still used by STlink SWD.
	//I didn't be successful to get PB3 freeed, and I had to reengineered this new solution, like
	//do not use interrupts to CAPS and KANA LEDs, and, for example, I put them inside systicks task,
	//that I suppose it is a better solution.
	gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, 0);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	//Debug & performance measurement
	msxmap object;
	object.general_debug_setup();

	serial_setup();

	//User messages
	usart_send_string((uint8_t*)"\r\n\n\nMSX Keyboard subsystem Emulator");

	usart_send_string((uint8_t*)"\r\n\nBooting...\r\nBuilt on ");
	//printf("Built on %s %s\n\n", __DATE__, __TIME__);
	usart_send_string((uint8_t*)__DATE__);
	usart_send_string((uint8_t*)" ");
	usart_send_string((uint8_t*)__TIME__);
	usart_send_string((uint8_t*)"\r\n\n");


	//User messages
	usart_send_string((uint8_t*)"Configuring:\r\n- 5V compatible pin ports and interrupts to interface to MSX;\n\r");

	object.msx_interface_setup();

	// GPIO C13 is the onboard LED
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO13);
	// Enable the led. It is active LOW, but the instruction was omitted, since 0 is the default.

	// GPIOB5 is a 3.3V pin. I can not use it with real MSX. In aim to reduce wasted power, let avoid let it floating,
	// so lets fix a state: Set it to 1 (set bit).
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO5);
	gpio_set(GPIOB, GPIO5); //pull up (internal) resistor

	usart_send_string((uint8_t*)"- SysTick;\r\n");

	systick_setup();
	
	//User messages
	usart_send_string((uint8_t*)"- High resolution timer2;\r\n");

	// Now configure High Resolution Timer for micro second resolution Delay
	tim2_setup();

	usart_send_string((uint8_t*)"- Ports config locking.\r\n");

	usart_send_string((uint8_t*)"\r\nBoot complete! Press ? to show menu.\r\n");
	
	//Updated from menu (serial console).
	scan_pointer = INIT_SCAN_POINTER;									//Starts with 32KHz
	delay_to_read_x_scan = INIT_DELAY_TO_READ_X_SCAN;	//Starts with 3.65μs
	init_scancount = 0;
	end_scancount  = 0x08;	//8 for HB8000, 9 for Expert and 0A for full MSX keyboards
	caps_line = 0x0B;				//Starts with caps led blinking
	kana_line = 0x0B;				//Starts with Scroll led blinking
	wait_flag = false;			//Starts with running scan

	/*********************************************************************************************/
	/************************************** Main Loop ********************************************/
	/*********************************************************************************************/
	for(;;)
	{
		//The functionality running in main loop is to control serial communication and interact with user
		uint8_t mountstring[60];

		usart_send_string((uint8_t*)"\r\n> ");	//put prompt
		while (!serial_available_get_char())
			//wait here until new char is available at serial port, but print the changes info of received key
			while(isr_string_ring.get_ptr != isr_string_ring.put_ptr)
				serial_put_char((uint8_t)ring_get_ch(&isr_string_ring, &bin));
		uint8_t ch = serial_get_char();
		switch (ch)
		{
			case '?': //if (ch == '?')
			{
				usart_send_string((uint8_t*)"(?) Available options\r\n1) General:\r\n");
				usart_send_string((uint8_t*)"   r (Show Running config);\r\n");
				usart_send_string((uint8_t*)"   c (Caps Lock line <- On/Off/Blink);\r\n");
				usart_send_string((uint8_t*)"   k (Kana line      <- On/Off/Blink);\r\n2) Scan related:\r\n");
				usart_send_string((uint8_t*)"   s (Scan submenu - Set first [Y Begin] and last [Y End] colunms to scan);\r\n");
				usart_send_string((uint8_t*)"   + (Increase scan rate);\r\n");
				usart_send_string((uint8_t*)"   - (Decrease scan rate);\r\n");
				usart_send_string((uint8_t*)"   p (Toggle pause scan);\r\n");
				usart_send_string((uint8_t*)"   n (Next step colunm scan)                        <= when scan is paused;\r\n");
				usart_send_string((uint8_t*)"   Space (One shot scan, from [Y Begin] to [Y End]) <= when scan is paused;\r\n");
				usart_send_string((uint8_t*)"3) Time to read X_Scan (after Y_Scan) update:\r\n");
				usart_send_string((uint8_t*)"   < (decrease by 0.25μs);\r\n");
				usart_send_string((uint8_t*)"   > (increase by 0.25μs).\r\n");
				break;
			}

			case 'r': //else if (ch == 'r')
			{
				//Show Running config. Only print.
				usart_send_string((uint8_t*)"(r) Running config:\r\nScan rate: ");
				usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
				if(wait_flag)
				{
					usart_send_string((uint8_t*)"Hz;\r\nScan is PAUSED;");
				}
				else
				{
					usart_send_string((uint8_t*)"Hz;\r\nScan is RUNNING;");
				}
				usart_send_string((uint8_t*)"\r\nScan begins [Y Begin] at 0x");
				conv_uint8_to_2a_hex(init_scancount, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)" and ends [Y End] at 0x");
				conv_uint8_to_2a_hex(end_scancount, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)";\r\nDelay to read X_Scan (after Y_Scan update): ");
				usart_send_string((uint8_t*)&TIME_TO_READ_X[delay_to_read_x_scan][0]);
				usart_send_string((uint8_t*)"μs\r\nCaps Line: Current value = ");
				if (caps_line == 0)
					usart_send_string((uint8_t*)"0 (active)");
				else if (caps_line == 1)
					usart_send_string((uint8_t*)"1 (off)");
				else if (caps_line == 0x0B)
					usart_send_string((uint8_t*)"Blink");
				usart_send_string((uint8_t*)";\r\nKana Line: Current value = ");
				if (kana_line == 0)
					usart_send_string((uint8_t*)"0 (active);\r\n");
				else if (kana_line == 1)
					usart_send_string((uint8_t*)"1 (off);\r\n");
				else if (kana_line == 0x0B)
					usart_send_string((uint8_t*)"Blink;\r\n");
				break;
			}	//case 'r': //else if (ch == 'r')

			case 'c': //else if (ch == 'c')
			{
				//Caps Lock. Print current one.
				usart_send_string((uint8_t*)"(c) Caps Line - Set to Active (0), Off (1) or Blink (B).\r\nCurrent value = ");
				conv_uint8_to_2a_hex(caps_line, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)";\r\n");
				usart_send_string((uint8_t*)"Enter 0, 1 or B to update: ");
				ch = 0xFF;
				while (  (ch != '0') && (ch != '1') && (ch != 'B') )
				{
					while (!serial_available_get_char()) __asm("nop");	//wait here until new char is available at serial port
					ch = serial_get_char();
					if (ch >= 'a')
						ch &= 0x5F; //To capital
				}
				serial_put_char(ch);
				usart_send_string((uint8_t*)"\r\n");
				mountstring[0] = '0';
				mountstring[1] = ch;
				mountstring[2] = '\0';
				caps_line = conv_2a_hex_to_uint8(&mountstring[0], 0);
				break;
			}

			case 'k':	//else if (ch == 'k')
			{
				//Kana Line (Related to PS/2 Scroll Lock).
				usart_send_string((uint8_t*)"(k) Kana Line - Set to Active (0), Off (1) or Blink (B)\r\nCurrent value = ");
				conv_uint8_to_2a_hex(kana_line, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)";\r\n");
				usart_send_string((uint8_t*)"Enter 0, 1 or B to update: ");
				ch = 0xFF;
				while (  (ch != '0') && (ch != '1') && (ch != 'B') )
				{
					while (!serial_available_get_char()) __asm("nop");	//wait here until new char is available at serial port
					ch = serial_get_char();
					if (ch >= 'a')
						ch &= 0x5F; //To capital
				}
				serial_put_char(ch);
				usart_send_string((uint8_t*)"\r\n");
				mountstring[0] = '0';
				mountstring[1] = ch;
				mountstring[2] = '\0';
				kana_line = conv_2a_hex_to_uint8(&mountstring[0], 0);
				break;
			}	//case 'k':	//else if (ch == 'k')

			case 's':	//if (ch == 's')
			{
				//Update Y Begin and End
				usart_send_string((uint8_t*)"(s) Scan Sub menu:");
				usart_send_string((uint8_t*)"\r\n         ^C or Enter now aborts;");
				usart_send_string((uint8_t*)"\r\n         b ([Y Begin] - Update the value) Current one = 0x");
				conv_uint8_to_2a_hex(init_scancount, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)";\r\n         e ([Y End] - Update the value) Current one = 0x");
				conv_uint8_to_2a_hex(end_scancount, &mountstring[0]);
				serial_put_char(mountstring[1]);
				usart_send_string((uint8_t*)".\r\n>> ");
				while (!serial_available_get_char()) __asm("nop");	//wait here until new char is available at serial port
				ch = serial_get_char();
				if (ch != ('\r' || '\03'))
				{
					//Operation not aborted
					if (ch == 'b')
					{
						//Y Begin. Print current one: init_scancount
						usart_send_string((uint8_t*)"Scan begins at colunm 0x");
						conv_uint8_to_2a_hex(init_scancount, &mountstring[0]);
						serial_put_char(mountstring[1]);
						usart_send_string((uint8_t*)". Enter 0-F to update: ");
						ch = 0xFF;
						while ( ! ( ((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')) ) )
						{
							while (!serial_available_get_char())
								__asm("nop");	//wait here until new char is available at serial port
							ch = serial_get_char();
							if (ch >= 'a')
								ch &= 0x5F; //To capital
						}
						serial_put_char(ch);
						usart_send_string((uint8_t*)"\r\n");
						mountstring[0] = '0';
						mountstring[1] = ch;
						mountstring[2] = '\0';
						init_scancount = conv_2a_hex_to_uint8(&mountstring[0], 0);
					}
					else if (ch == 'e')
					{
						//Y End. Print current one: end_scancount
						usart_send_string((uint8_t*)"Scan ends at colunm 0x");
						conv_uint8_to_2a_hex(end_scancount, &mountstring[0]);
						serial_put_char(mountstring[1]);
						usart_send_string((uint8_t*)". Enter 0-F to update: ");
						ch = 0xFF;
						while ( ! ( ((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')) ) )
						{
							while (!serial_available_get_char()) __asm("nop");	//wait here until new char is available at serial port
							ch = serial_get_char();
							if (ch >= 'a')
								ch &= 0x5F; //To capital
						}
						serial_put_char(ch);
						usart_send_string((uint8_t*)"\r\n");
						mountstring[0] = '0';
						mountstring[1] = ch;
						mountstring[2] = '\0';
						end_scancount = conv_2a_hex_to_uint8(&mountstring[0], 0);
					}	//else if (ch == 'e')
				}	//if (ch != ('\r' && '\03'))
				else
				{
					//Aborted
					usart_send_string((uint8_t*)"\r\nOperation aborted!\r\n");
				}
				break;
			}	//case 's':

			case '+':
			{
				if(scan_pointer <= SCAN_POINTER_SIZE-2)
				{
					scan_pointer ++;
					systick_update(scan_pointer);	//t_sys_timer.cpp has a table with systick reload
					usart_send_string((uint8_t*)"(+) New scan frequency applied: ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"\r\n");
				}
				else
				{
					usart_send_string((uint8_t*)"Maximum scan frequency unchanged: Already working: ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"\r\n");
				}
				break;
			}	//case '+':

			case '-':
			{
				if(scan_pointer)
				{
					scan_pointer --;
					systick_update(scan_pointer);	//t_sys_timer.cpp has a table with systick reload
					usart_send_string((uint8_t*)"(-) New scan frequency applied: ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"Hz\r\n");
				}
				else
				{
					usart_send_string((uint8_t*)"Minimum scan frequency unchanged: Already workimg at ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"Hz\r\n");
				}
				break;
			}	//case '-':

			case 'p':	//else if (ch == 'p')
			{
				if(wait_flag ^= true)
				{
					usart_send_string((uint8_t*)"(p) (Toggle pause scan): Scan is paused\r\n");
				}
				else
				{
					usart_send_string((uint8_t*)"p (Toggle pause scan): Scan is running. Config:");
					usart_send_string((uint8_t*)"\r\nScan rate: ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"Hz;\r\nScan is beginning at 0x");
					conv_uint8_to_2a_hex(init_scancount, &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)" [Y Begin] and ending at 0x");
					conv_uint8_to_2a_hex(end_scancount, &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)" [Y End].\r\n");
				}
				break;
			}	//case 'p':	//else if (ch == 'p')

			case ' ':
			{
				if(wait_flag)
				{
					usart_send_string((uint8_t*)"(" ") One shot scan with the configuration:");
					usart_send_string((uint8_t*)"\r\nScan rate: ");
					usart_send_string((uint8_t*)&SPEED_SELEC[scan_pointer][0]);
					usart_send_string((uint8_t*)"Hz;\r\nScan will begin at 0x");
					conv_uint8_to_2a_hex(init_scancount, &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)" [Y Begin] and will end at 0x");
					conv_uint8_to_2a_hex(end_scancount, &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)" [Y End].\r\n");
					single_sweep = true;
					wait_flag = false;
					y_scan = init_scancount;
				}
				break;
			}	//case ' '

			case 'n':
			{
				if(wait_flag)
				{
					usart_send_string((uint8_t*)"(n) Next step colunm scan. This one is 0x");
					conv_uint8_to_2a_hex(y_scan, &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)"; then next will be 0x");
					if ((y_scan + 1) <= end_scancount)
						conv_uint8_to_2a_hex((y_scan + 1), &mountstring[0]);
					else 
						conv_uint8_to_2a_hex((init_scancount), &mountstring[0]);
					serial_put_char(mountstring[1]);
					usart_send_string((uint8_t*)".\r\n");
					single_step = true;
					wait_flag = false;
				}	//if(wait_flag)
				break;
			}	//case 'n'

			case '>':
			{
				if(delay_to_read_x_scan <= DELAY_TO_READ_SIZE-2)
				{
					delay_to_read_x_scan ++;
					usart_send_string((uint8_t*)"(>) New delay to read X_Scan (after Y_Scan update) applied: ");
					usart_send_string((uint8_t*)&TIME_TO_READ_X[delay_to_read_x_scan][0]);
					usart_send_string((uint8_t*)"μs\r\n");
				}
				else
				{
					usart_send_string((uint8_t*)"(>) Maximum delay to read X_Scan unchanged: Already workimg at ");
					usart_send_string((uint8_t*)&TIME_TO_READ_X[delay_to_read_x_scan][0]);
					usart_send_string((uint8_t*)"μs\r\n");
				}
				break;
			}	//case '>':

			case '<':
			{
				if(delay_to_read_x_scan)
				{
					delay_to_read_x_scan --;
					usart_send_string((uint8_t*)"(<) New delay to read X_Scan (after Y_Scan update) applied: ");
					usart_send_string((uint8_t*)&TIME_TO_READ_X[delay_to_read_x_scan][0]);
					usart_send_string((uint8_t*)"μs\r\n");
				}
				else
				{
					usart_send_string((uint8_t*)"(<) Minimum delay to read X_Scan unchanged: Already workimg at ");
					usart_send_string((uint8_t*)&TIME_TO_READ_X[delay_to_read_x_scan][0]);
					usart_send_string((uint8_t*)"μs\r\n");
				}
				break;
			}	//case '<':
		}	//switch (ch)
	}	//for(;;)
	return 0; //Suppose never reach here
} //int main(void)

