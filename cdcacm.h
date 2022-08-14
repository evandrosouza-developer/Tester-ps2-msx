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

/* This file implements a the USB Communications Device Class - Abstract
 * Control Model (CDC-ACM) as defined in CDC PSTN subclass 1.2.
 *
 * The device's unique id is used as the USB serial number string.
 */
//Use Tab width=2

#if !defined CDCACM_H
#define CDCACM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "serial.h"


void cdcacm_init(void);
void set_nak_endpoint(uint8_t);
void clear_nak_endpoint(uint8_t);
void first_put_ring_content_onto_ep(struct sring*, uint8_t);
/* Returns current usb configuration, or 0 if not configured. */
int cdcacm_get_config(void);

#ifdef __cplusplus
}
#endif

#endif  //#if !defined CDCACM_H
