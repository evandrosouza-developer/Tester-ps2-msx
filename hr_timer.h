/** @defgroup 05 High hr_timer.c / hr_timer.h
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

//Definitions of TIMER stateful machine
enum TIM_ST_MACH{
  TIME_CAPTURE =                 0x420,  //1056 "Normal" state
  SEND_ST_BIT_2,                         //1057
  SEND_ST_BIT_3,                         //1058
};

/**
 * @brief Sets up the struct sring ring.
 * @param Timer to be used.
 * @return void
 */
void tim_hr_setup(uint32_t);


/**
 * @brief Inits the struct sring ring.
 * @param Timer to be used.
 * @param pointer to already defined buffer.
 * @param size of the already defined buffer.
 * @return void
 */
//void usb_tx_usec(uint32_t, uint16_t);


/**
 * @brief Inserts a delay with a resolution of a microsecond and call the desired function.
 * @param Timer to be used.
 * @param delay (in microseconds).
 * @param pointer of the desired function to be called after time is up.
 * @return void
 */
void delay_usec(uint32_t, uint16_t, void next_step (void));


/**
 * @brief Inserts a delay with a resolution of a quarter microsecond and call the desired function.
 * @param Timer to be used.
 * @param delay (in quarters of microsecond).
 * @param pointer of the desired function to be called after time is up.
 * @return void
 */
void delay_qusec(uint32_t, uint16_t, void next_step (void));


/**
 * @brief Starts the Timer to be used to make a time capture.
 * @param Timer to be used.
 * @return void
 */
void prepares_capture(uint32_t timer_peripheral);


#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus

#endif	//#define HR_TIMER_H
