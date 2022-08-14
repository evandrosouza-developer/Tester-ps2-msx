/*
 * This file is part of the MSX Keyboard Subsystem Emulator project.
 *
 * Copyright (C) 2022 Evandro Souza <evandro.r.souza@gmail.com>
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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>

#include "msxmap.h"
#include "system.h"
#include "serial.h"

//Variáveis globais: Visíveis por todo o contexto do programa
extern uint32_t systicks;														//Declared on sys_timer.cpp
extern uint8_t init_scancount, end_scancount;				//Declared on sys_timer.cpp
extern uint8_t y_scan;															//Declared on sys_timer.cpp
extern bool wait_flag, single_step, single_sweep;		//Declared on sys_timer.cpp
extern uint8_t inactivity_cycles[SCAN_POINTER_SIZE];//Declared on sys_timer.cpp
extern volatile uint64_t TIM2_Update_Cnt;						//Declarated on hr_timer.c
extern volatile uint64_t u64_TIM2_Cnt;							//Declarated on hr_timer.c
extern uint16_t init_inactivity_cycles[SCAN_POINTER_SIZE];//Declared on tester-ps2-msx.cpp
extern uint32_t scan_pointer;												//Declared on tester-ps2-msx.cpp
uint8_t msx_X;

volatile uint32_t previous_y_systick[ 16 ] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//MSX Keyboard - Used to signalize status change in MSX matrix
uint8_t msx_matrix[ 16 ] =  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
														 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	//The index is used to store Y

uint8_t mountISRstring[SERIAL_RING_BUFFER_SIZE];
struct sring isr_string_ring;

//Prototypes
void isr_string_concat(uint8_t*, struct sring *);
void ring_init(struct sring *, uint8_t*);


void ring_init(struct sring *ring, uint8_t *buf)
{
	ring->data = buf;
	ring->put_ptr = 0;
	ring->get_ptr = 0;
}


void msxmap::msx_interface_setup(void)
{
#if MCU == STM32F103
	//Not the STM32 default: Pull up;
	gpio_set(X7_port,
	X7_pin_id | X6_pin_id | X5_pin_id | X4_pin_id | X3_pin_id | X2_pin_id | X1_pin_id | X0_pin_id);

	//Init input port B15:8
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
#endif	//#if MCU == STM32F103

#if MCU == STM32F401
	//Not the STM32 default: Pull up;
	//Init input port B15:8
	gpio_mode_setup	(X7_port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, 
	X7_pin_id | X6_pin_id | X5_pin_id | X4_pin_id | X3_pin_id | X2_pin_id | X1_pin_id | X0_pin_id);		


	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC3 MSX 8255 Pin 17)
	gpio_mode_setup	(Y3_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Y3_pin_id);
	gpio_set_output_options	(Y3_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Y3_pin_id);
	gpio_port_config_lock(Y3_port, Y3_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC2 MSX 8255 Pin 16)
	gpio_mode_setup	(Y2_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Y_pin_id);
	gpio_set_output_options	(Y2_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Y_pin_id);
	gpio_port_config_lock(Y2_port, Y_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC1 MSX 8255 Pin 15)
	gpio_mode_setup	(Y2_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Y_pin_id);
	gpio_set_output_options	(Y2_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Y_pin_id);
	gpio_port_config_lock(Y1_port, Y1_pin_id);

	// GPIO pins for MSX keyboard Y scan (PC3:0 of the MSX 8255 - PC0 MSX 8255 Pin 14)
	gpio_mode_setup	(Y0_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Y0_pin_id);
	gpio_set_output_options	(Y0_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Y_pin_id);
	gpio_port_config_lock(Y0_port, Y0_pin_id);

	// GPIO pin for Oscilloscope sync
	gpio_mode_setup	(Y_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Y_pin_id); // PC0 (MSX 8255 Pin 14)
	gpio_set_output_options	(Y_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Y_pin_id);
	gpio_port_config_lock(Y_port, Y_pin_id);

	// GPIO pins for CAPS & KANA
	gpio_mode_setup	(CAPS_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, CAPS_pin_id);
	gpio_set_output_options	(CAPS_port, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, Y_pin_id);
	gpio_port_config_lock(CAPS_port, CAPS_pin_id);

	gpio_mode_setup	(KANA_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, KANA_pin_id);
	gpio_set_output_options	(KANA_port, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, KANA_pin_id);
	gpio_port_config_lock(KANA_port, KANA_pin_id);
#endif	//#if MCU == STM32F401

	// Initialize ring buffer for readings of DUT inside isr.
	ring_init(&isr_string_ring, mountISRstring);

	//Init init_inactivity_cycles[SCAN_POINTER_SIZE]
	for(uint16_t i = 0; i < SCAN_POINTER_SIZE; i++)
		{
			init_inactivity_cycles[i] = 0;	//working values
			inactivity_cycles[i] = 0;	//working values
		}
}


void msxmap::general_debug_setup(void)
{
#if MCU == STM32F103
	// GPIO C13 is the onboard LED
	gpio_set_mode(EMBEDDED_LED_port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, EMBEDDED_LED_pin);
	// Enable the led. It is active LOW, but the instruction was omitted, since 0 is the default.

	// GPIOB5 is a 3.3V pin. I can not use it with real MSX. In aim to reduce wasted power, let avoid let it floating,
	// so lets fix a state: Set it to 1 (set bit).
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO5);
	gpio_set(GPIOB, GPIO5); //pull up (internal) resistor

	gpio_set_mode(SYSTICK_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, SYSTICK_pin_id);
	gpio_set(SYSTICK_port, SYSTICK_pin_id); //Default condition is "1"

	gpio_set_mode(Xint_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, Xint_pin_id); // PC2 e 3 (MSX 8255 Pin 17)
	gpio_set(Xint_port, Xint_pin_id); //Default condition is "1"
	
	gpio_set_mode(INT_TIM2_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, TIM2UIF_pin_id); // PC2 e 3 (MSX 8255 Pin 17)
	gpio_set(INT_TIM2_port, TIM2UIF_pin_id); //Default condition is "1"
#endif	//#if MCU == STM32F103

#if MCU == STM32F401
	// GPIO C13 is the onboard LED
	gpio_mode_setup	(EMBEDDED_LED_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, EMBEDDED_LED_pin);
	gpio_set_output_options	(EMBEDDED_LED_port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, EMBEDDED_LED_pin);
	// Enable the led. It is active LOW, but the instruction was omitted, since 0 is the default.

	gpio_set(SYSTICK_port, SYSTICK_pin_id); //Default condition is "1"
	gpio_mode_setup	(SYSTICK_port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SYSTICK_pin_id);
	gpio_set_output_options	(SYSTICK_port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, SYSTICK_pin_id);

	gpio_set(Xint_port, Xint_pin_id); //Default condition is "1"
	gpio_mode_setup	(Xint_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Xint_pin_id);
	gpio_set_output_options	(Xint_port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, Xint_pin_id);
	
	gpio_set(INT_TIM2_port, TIM2UIF_pin_id); //Default condition is "1"
	gpio_mode_setup	(INT_TIM2_port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, TIM2UIF_pin_id);							// PC2 e 3 (MSX 8255 Pin 17)
	gpio_set_output_options	(INT_TIM2_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, Xint_pin_id);
#endif	//#if MCU == STM32F401
}


// Concat an ASCIIZ (uint8_t) string on ISR String Mounting buffer.
void isr_string_concat(uint8_t *string_org, struct sring *str_mount_buff)
{
	uint8_t ch;
	uint16_t total_in_buffer;

	uint16_t i = 0;
	do
	{
		ch = string_org[i];
		if (ch == 0) //quit do-while
			break;
		total_in_buffer = ring_put_ch(str_mount_buff, ch);
		i++;	//points to next char on the stringorg
	}	while(total_in_buffer <  SERIAL_RING_BUFFER_SIZE);
}


//This routine will be called from delay_usec (from t_sys_timer / t_hr_timer)
void portXread(void)
{
	uint8_t mountstring[6];					//Used in usart_send_string()
	uint16_t wmsx_X;

	//To be measured the real time from column *Y_ writing to  reading, by putting an oscilloscope at pin A1
	GPIO_BSRR(Xint_port) = Xint_pin_id; //Back to default condition ("1")

	// Read the MSX keyboard X answer through GPIO pins B15:6
	wmsx_X = (gpio_port_read(GPIOB) >> 6); //Read bits B15, B14, B13, B12, B9, B8, B7 and B6 (1111.0011.1100.0000 F3C0)
	msx_X = (uint8_t)((wmsx_X >> 2) & 0xF0) | (uint8_t)(wmsx_X & 0x0F);
	
	if (msx_X != msx_matrix[y_scan])	//Print info if ps2-MSX adapter has updated data of this y_scan
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
	if (!wait_flag)
	{//Update here to next valid scan
		y_scan++;
		if (y_scan > end_scancount)
		{
			y_scan = init_scancount;
			inactivity_cycles[scan_pointer] = init_inactivity_cycles[scan_pointer];
			if(single_sweep)
			{//Last colunm to scan: Back to paused state
				single_sweep = false;
				wait_flag = true;		//Do this last one and, after portXread executed, stop single_sweep
			}	//if(single_sweep)
		}
	}
	if (single_step)
	{//Run this time and returns to paused state
		single_step = false;
		wait_flag = true;
	}
}
