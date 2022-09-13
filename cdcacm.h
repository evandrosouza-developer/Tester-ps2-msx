/** @defgroup USB peripheral API
 *
 * @ingroup infrastructure_apis
 *
 * @brief <b>PS/2 to MSX keyboard Converter Enviroment</b>
 *
 * @version 1.0.0
 *
 * @author @htmlonly &copy; @endhtmlonly 2022
 * Evandro Souza <evandro.r.souza@gmail.com>
 * @author @htmlonly &copy; @endhtmlonly 2010
 * Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * @date 01 September 2022
 *
 * This library supports the USART with DMA in the STM32F4 and STM32F1
 * series of ARM Cortex Microcontrollers by ST Microelectronics.
 *
 * LGPL License Terms @ref lgpl_license
 */

/*
 * This file is part of the PS/2 to MSX keyboard Converter and 
 * MSX Keyboard Subsystem Emulator projects, based on libopencm3 project.
 *
 * Copyright (C) 2022 Evandro Souza <evandro.r.souza@gmail.com>
 *
 * Based on CDCACM example of:
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

/**@{*/

#if !defined CDCACM_H
#define CDCACM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "serial.h"


static const struct usb_cdc_line_coding line_coding = {
  .dwDTERate = 115200,
  .bCharFormat = USB_CDC_1_STOP_BITS,
  .bParityType = USB_CDC_NO_PARITY,
  .bDataBits = 8
};


/**
 * @brief Inits the cdcacm sub system.
 * @param none
 * @return none
 */
void cdcacm_init(void);


/**
 * @brief Sets the specified endpoint to Not Acknowledge, to disables communication.
 * @param endpoint with direction bit.
 * @return none
 */
void set_nak_endpoint(uint8_t);


/**
 * @brief Clears the specified endpoint Not Acknowledg, to allow communication.
 * @param endpoint with direction bit.
 * @return none
 */
void clear_nak_endpoint(uint8_t);


/**
 * @brief Starts a communication pipe with an IN Endpoint, to allow that the USB callbacks can take control of the pipe.
 * @param pointer to struct sring.
 * @param endpoint with direction bit.
 * @return none
 */
void first_put_ring_content_onto_ep(struct sring*, uint8_t);


/* Returns current usb configuration, or 0 if not configured. */
/**
 * @brief Returns current usb configuration, or 0 if not configured.
 * @param none.
 * @return Returns current usb configuration, or 0 if not configured.
 */
int cdcacm_get_config(void);


/**
 * @brief Disables the USB to make host disconnect.
 * @param none.
 * @return none
 */
void disable_usb(void);



#ifdef __cplusplus
}
#endif

#endif  //#if !defined CDCACM_H
