/*
 * This file is part of the PS/2 to MSX keyboard Converter and
 * MSX Keyboard Subsystem Emulator projects, based on libopencm3 project.
 *
 * Copyright (C) 2022 Evandro Souza <evandro.r.souza@gmail.com>
 * Copyright (C) 2015 Karl Palsson <karlp@tweak.net.au>
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

/* Linker script for STM32F401xC, 256k flash, 64k RAM. */

/* Define memory regions. */
MEMORY
{
	ram (rwx)				: ORIGIN = 0x20000000, LENGTH = 64K
	rom (rx)				: ORIGIN = 0x08000000, LENGTH = 256K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

