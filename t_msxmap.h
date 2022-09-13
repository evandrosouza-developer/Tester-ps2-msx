/** @defgroup Keyboard Conversion and MSX answering Module API
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
 * This library supports the setup pin ports and process the read data from 
 * PS/2 to MSX Keyboard Converter to be presented at the main menu loop,
 * to both the STM32F4 and STM32F1 series of ARM Cortex Microcontrollers
 * by ST Microelectronics.
 *
 * LGPL License Terms @ref lgpl_license
 */
/*
 * This file is part of the MSX Keyboard Subsystem Emulator project.
 *
 * Copyright (C) 2022 Evandro Souza <evandro.r.souza@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef T_MSXMAP_H
#define T_MSXMAP_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

//Use Tab width=2


class msxmap
{
private:

public:
  /**
   * @brief Properly sets up the all pins that interfaces to PS/2 to MSX Keyboard Converter.
   * @param none
   * @return none
   */
  void msx_interface_setup(void);


  /**
   * @brief Properly sets up the all debug output pins.
   * @param none
   * @return none
   */
  void general_debug_setup(void);
};

  void portXread(void);

#endif  //#ifndef T_MSXMAP_H
