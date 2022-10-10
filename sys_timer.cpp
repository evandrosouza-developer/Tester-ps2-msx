/** @addtogroup sys_timer System_Timer
 *
 * @ingroup infrastructure_apis
 *
 * @file sys_timer.cpp System Timer: Marks led blinking, depending on the selected scan rate. Starts reading PS/2 to MSX adapter.
 *
 * @brief <b>System Timer: Marks led blinking, depending on the selected scan rate. Starts reading PS/2 to MSX adapter.</b>
 *
 * @version 1.0.0
 *
 * @author @htmlonly &copy; @endhtmlonly 2022
 * Evandro Souza <evandro.r.souza@gmail.com>
 *
 * @date 01 September 2022
 *
 * This library supports the ARM System Timer in the STM32F4 and STM32F1
 * series of ARM Cortex Microcontrollers by ST Microelectronics.
 * In Tester-PS2_MSX, it is responsible to manage the timming and generate 
 * the scan pattern and starts the delay of the X line readings.
 *
 * LGPL License Terms ref lgpl_license
 */
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

#include "sys_timer.h"


//Global vars
volatile uint32_t systicks, tickscaps;
volatile bool ticks_keys, wait_flag, single_sweep, single_step;
volatile uint8_t scancount_init, scancount_end, y_scan;
volatile uint8_t caps_line, kana_line;
volatile uint16_t last_ps2_fails=0;
uint8_t inactivity_cycles[SCAN_POINTER_SIZE];
extern uint32_t scan_pointer;                     //Declared on tester-ps2-msx.cpp
extern uint32_t delay_to_read_x_scan;             //Declared on tester-ps2-msx.cpp
extern volatile uint64_t u64_TIM_HR_Cnt;          //Declarated on t_hr_timer.c
extern volatile uint64_t TIM_HR_Update_Cnt;       //Declarated on t_hr_timer.c

const uint32_t y_bits[ 16 ] = {Y_0, Y_1, Y_2, Y_3, Y_4, Y_5, Y_6, Y_7, Y_8, Y_9, Y_A, Y_B, Y_C, Y_D, Y_E, Y_F};

#if MCU == STM32F103
const uint32_t SPEED_SYSTICK_RELOAD[SCAN_POINTER_SIZE] = {9000000, 4500000, 2250000, 1125000, 562500, 281250, 140625, 70312,
                                                          35156, 17578, 8789, 4394, 2197, 1098, 548, 277, 150};

const uint16_t SPEED_SYSTICK_DIVISOR[SCAN_POINTER_SIZE] = {2, 2, 3, 4, 7, 12, 20, 36, 64, 115, 208, 
                                                           383, 707, 1315, 2460, 4565, 7938};
#endif  //#if MCU == STM32F103

#if MCU == STM32F401
const uint32_t SPEED_SYSTICK_RELOAD[SCAN_POINTER_SIZE] = {9333332, 4666666, 2333332, 1166666, 583332, 291666, 145832, 72916,
                                                          36457, 18228, 9114, 4556, 2278, 1138, 567, 290, 154};

const uint16_t SPEED_SYSTICK_DIVISOR[SCAN_POINTER_SIZE] = {2, 2, 3, 4, 7, 12, 20, 36, 64, 115, 208,
                                                           383, 707, 1314, 2461, 4506, 7966};
#endif  //#if MCU == STM32F401

const uint8_t TIME_TO_READ_X_TABLE[DELAY_TO_READ_SIZE] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};



void systick_setup(void)
{
  //Make sure systick doesn't interrupt more important interrupts
  nvic_set_priority(NVIC_SYSTICK_IRQ, IRQ_PRI_SYSTICK);

  systick_set_reload(SPEED_SYSTICK_RELOAD[INIT_SCAN_POINTER]-1);
  
  systicks = 0; //systick_clear
  ticks_keys = false;
  wait_flag = false;
  gpio_clear(CAPS_port, CAPS_pin_id);
  gpio_set(KANA_port, KANA_pin_id);
  caps_line = 0x0B; //Starts blinking
  kana_line = 0x0B; //Starts blinking

  systick_interrupt_enable();

  /* Start counting. */
  systick_counter_enable();
  
  y_scan = scancount_init;
}


void systick_update(uint8_t s_pointer)
{
  /* Stop counting. */
  systick_counter_disable();
  
  systick_set_reload(SPEED_SYSTICK_RELOAD[s_pointer]-1);
  
  /* Start counting. */
  systick_counter_enable();
  
  y_scan = scancount_init;
}



void write_to_Y_port(uint32_t to_out_in_Y_port)
{
  GPIO_BSRR(Y_Begin_Mark_port) = y_bits[to_out_in_Y_port]; //Atomic GPIOA update => Update scan for the column
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
    gpio_toggle(EMBEDDED_LED_port, EMBEDDED_LED_pin);

    //CapsLock and Kana Line Control
    switch (caps_line)
    {
      case 0x00:
        gpio_clear(CAPS_port, CAPS_pin_id);
        break;
      case 0x01:
        gpio_set(CAPS_port, CAPS_pin_id);
        break;
      case 0x0B:
        gpio_toggle(CAPS_port, CAPS_pin_id);
        break;
    }
    switch (kana_line)
    {
      case 0x00:
        gpio_clear(KANA_port, KANA_pin_id);
        break;
      case 0x01:
        gpio_set(KANA_port, KANA_pin_id);
        break;
      case 0x0B:
        gpio_toggle(KANA_port, KANA_pin_id);
        break;
    }
  } //if(tickscaps >= SPEED_SYSTICK_DIVISOR[scan_pointer])
    
  if( !inactivity_cycles[scan_pointer] || (single_step || single_sweep) )
  {
    //To be capable of read the Converter even on wait state
    delay_qusec(TIM_HR, TIME_TO_READ_X_TABLE[delay_to_read_x_scan], portXread); //3.6us is the target. As the timer2 is ticking at 4MHz (250ns period)
    if (!wait_flag)
    {
      //Put Y_Scan on port
      GPIO_BSRR(Y_Begin_Mark_port) = y_bits[y_scan]; //Atomic GPIOA update => Update scan for the column
      if (y_scan == scancount_init)
      {
        //clear Y_Begin_Mark_pin (Y_Begin_Mark_pin is high by default. Only if (y_scan == scancount_init) it will be low)
        GPIO_BSRR(Y_Begin_Mark_port) = (Y_Begin_Mark_pin << 16); //To trig an oscilloscope to the scan start
      }
    } //if (!wait_flag)
  } // if( !inactivity_cycles[scan_pointer] || (wait_flag || single_step || single_sweep) )
  if(inactivity_cycles[scan_pointer])
  {
    //It is here because it is not time to scan
    //Update here to next valid scan
    y_scan++;
    if (y_scan > scancount_end)
    {
      y_scan = scancount_init;
      inactivity_cycles[scan_pointer]--;
    }
  } //if( !inactivity_cycles[scan_pointer] || (wait_flag || single_step || single_sweep) )
}
