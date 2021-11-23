#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>

#include "t_sys_timer.h"
#include "serial.h"
#include "t_hr_timer.h"
#include "t_msxmap.h"
#include "t_port_def.h"
//Use Tab width=2


//Global vars
volatile uint32_t systicks, tickscaps;
volatile bool ticks_keys, wait_flag, single_sweep, single_step;
volatile uint8_t init_scancount, end_scancount, y_scan;
volatile uint8_t caps_line, kana_line;
volatile uint16_t last_ps2_fails=0;
volatile uint16_t fail_count;
extern bool ps2_keyb_detected;										//Declared on ps2stdhandl.c
extern bool update_ps2_leds, ps2numlockstate;			//Declared on msxmap.cpp
extern uint8_t keycode[4];												//Declared on msxmap.cpp
extern uint8_t formerkeycode[4];									//Declared on msxmap.cpp
extern bool mount_keycode_OK; 										//Declared on ps2stdhandl.c
extern uint32_t scan_pointer;											//Declared on tester-ps2-msx.cpp
extern uint32_t delay_to_read_x_scan;										//Declared on tester-ps2-msx.cpp
extern volatile uint64_t u64_TIM2_Cnt;						//Declarated on t_hr_timer.c
extern volatile uint64_t TIM2_Update_Cnt;					//Declarated on t_hr_timer.c

const uint32_t y_bits[ 16 ] = {Y_0, Y_1, Y_2, Y_3, Y_4, Y_5, Y_6, Y_7, Y_8, Y_9, Y_A, Y_B, Y_C, Y_D, Y_E, Y_F};

const uint32_t SPEED_SYSTICK_RELOAD[SCAN_POINTER_SIZE] = {9000000, 4500000, 2250000, 1125000, 562500, 281250, 140625, 70312,
																					35156, 17578, 8789, 4394, 2197, 1098, 548, 277, 150, 75};

const uint16_t SPEED_SYSTICK_DIVISOR[SCAN_POINTER_SIZE] = {2, 2, 3, 4, 7, 12, 20, 36, 64, 
																						115, 208, 383, 707, 1315, 2460, 4565, 7938, 15000};

const uint8_t TIME_TO_READ_X_TABLE[DELAY_TO_READ_SIZE] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};



void systick_setup(void)
{
	//Make sure systick doesn't interrupt
	nvic_set_priority(NVIC_SYSTICK_IRQ, 100);

	systick_set_reload(SPEED_SYSTICK_RELOAD[INIT_SCAN_POINTER]-1);
	
	systicks = 0; //systick_clear
	ticks_keys = false;
	wait_flag = false;
	caps_line = 0x0B;	//Starts blinking
	kana_line = 0x0B;	//Starts blinking

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
	
	y_scan = init_scancount;
}


void systick_update(uint8_t s_pointer)
{
	/* Stop counting. */
	systick_counter_disable();
	
	systick_set_reload(SPEED_SYSTICK_RELOAD[s_pointer]-1);
	
	/* Start counting. */
	systick_counter_enable();
}


/*************************************************************************************************/
/*************************************************************************************************/
/******************************************* ISR's ***********************************************/
/*************************************************************************************************/
/*************************************************************************************************/
void sys_tick_handler(void)
{
	systicks++;

	tickscaps++;
	if(tickscaps >= SPEED_SYSTICK_DIVISOR[scan_pointer]) //~6000 on 32K
	{ 
		tickscaps = 0;

		//C13 LED blink control
		gpio_toggle(GPIOC, GPIO13);

		//CapsLock and Kana Line Control
		switch (caps_line)
		{
			case 0x00:
				{
					gpio_clear(CAPS_port, CAPS_pin_id);
					break;
				}
			case 0x01:
				{
					gpio_set(CAPS_port, CAPS_pin_id);
					break;
				}
			case 0x0B:
				{
					gpio_toggle(CAPS_port, CAPS_pin_id);
					break;
				}
		}
		switch (kana_line)
		{
			case 0x00:
				{
					gpio_clear(KANA_port, KANA_pin_id);
					break;
				}
			case 0x01:
				{
					gpio_set(KANA_port, KANA_pin_id);
					break;
				}
			case 0x0B:
				{
					gpio_toggle(KANA_port, KANA_pin_id);
					break;
				}
		}
	}	//if(tickscaps >= SPEED_SYSTICK_DIVISOR[scan_pointer])
		
	if (!wait_flag)
	{
		//Queue keys processing:
		if (single_step)
		{
			//Run this time and returns to paused state
			single_step = false;
			wait_flag = true;
		}
		if ( (y_scan == end_scancount) && single_sweep )
		{
			//Last colunm to scan: Returns to paused state
			single_sweep = false;
			wait_flag = true;
		}

		delay_qusec(TIME_TO_READ_X_TABLE[delay_to_read_x_scan], portXread);	//3.6us is the target. As the timer2 is ticking at 4MHz (250ns period)
		//Put Y_Scan on port
		GPIO_BSRR(Y_port) = y_bits[y_scan]; //Atomic GPIOA update => Update scan for the column
		if (y_scan == init_scancount)
		{
			//clear Y_pin_id & Xint_pin_id
			GPIO_BSRR(Y_port) = (Y_pin_id << 16);	//To trig an oscilloscope to the scan start
		}
		u64_TIM2_Cnt = TIM2_Update_Cnt | TIM_CNT(TIM2);

		//IMPORTANT: The update to next valid y_scan was moved to portXread (t_msxmap.cpp) to fix print mismatch
	}	//if (!wait_flag)
}
