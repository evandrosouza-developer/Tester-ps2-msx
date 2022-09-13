/** @defgroup ARM System Timer peripheral API
 *
 * @ingroup infrastructure_apis
 *
 * @brief <b>PS/2 to MSX keyboard Converter Enviroment</b>
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
 * the scan pattern and starts the delay if X line readings.
 *
 * LGPL License Terms @ref lgpl_license
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

#ifndef SYSTIMER_H
#define SYSTIMER_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Sets up the system timer to defaults of the system.
 * @param none.
 * @return none
 */
void systick_setup(void);


/**
 * @brief Reprograms the system timer according to a lookup table.
 * @param index of the lookup table.
 * @return none
 */
void systick_update(uint8_t);


#ifdef __cplusplus
}
#endif
	
#endif	//#ifndef SYSTIMER_H
