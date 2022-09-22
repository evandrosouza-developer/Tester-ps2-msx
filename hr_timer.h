/** @defgroup 05 hr_timer High_Resolution_Timer
 *
 * @ingroup infrastructure_apis
 *
 * @file hr_timer.h High Resolution Timer routines.
 *
 * @brief <b>High Resolution Timer routines. Header file of hr_timer.c.</b>
 *
 * @version 1.0.0
 *
 * @author @htmlonly &copy; @endhtmlonly 2022
 * Evandro Souza <evandro.r.souza@gmail.com>
 * @author @htmlonly &copy; @endhtmlonly 2011
 * Piotr Esden-Tempski <piotr@esden.net>
 *
 * @date 01 September 2022
 *
 * This library supports the high resolution timer with DMA in the STM32F4
 * and STM32F1 series of ARM Cortex Microcontrollers by ST Microelectronics.
 * This module is used to do microsecond or quarter microsecond delays and 
 * measure time between pulses.
 *
 * LGPL License Terms ref lgpl_license
 */

/*
 * This file is part of PS/2 to MSX keyboard converter
 * This file was part of the libopencm3 project.
 *
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
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

#if !defined HR_TIMER_H
#define HR_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


#include "system.h"
#if (USE_USB == true)
#include "cdcacm.h"
#endif  //#if (USE_USB == true)


//Definitions of TIMER stateful machine
/**  Definitions of TIMER stateful machine.
 *
 * Starts with 1, as endpoint 0 is the default.
 @enum TIM_ST_MACH Timer stateful machine enum.*/
enum TIM_ST_MACH{
  TIME_CAPTURE =                 0x420,  //1056 "Normal" state
  SEND_ST_BIT_2,                         //1057
  SEND_ST_BIT_3,                         //1058
};


/**
 * @brief Sets up the High Resolution timer.
 * 
 * @param timer_peripheral Timer to be used.
 */
void tim_hr_setup(uint32_t timer_peripheral);


/**
 * @brief Inserts a delay with a resolution of a quarter microsecond and call the desired function.
 * @param timer_peripheral Timer to be used.
 * @param qusec delay (in quarters of microsecond).
 * @param next_step pointer of the desired function to be called after time is up.
 */
void delay_qusec(uint32_t timer_peripheral, uint16_t qusec, void next_step (void));


/**
 * @brief Starts the Timer to be used to make a time capture.
 * 
 * @param timer_peripheral Timer to be used.
 */
void prepares_capture(uint32_t timer_peripheral);


#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus

#endif	//#define HR_TIMER_H
