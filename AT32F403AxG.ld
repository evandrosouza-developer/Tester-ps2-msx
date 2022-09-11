/*
*****************************************************************************
**
**  File        : AT32F403AxG.ld
**
**  Abstract    : Linker script for AT32F403AxG Device with
**                1000KByte FLASH, 96KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Artery Tek AT32
**
**  Environment : Arm gcc toolchain
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/*
 * This file was part of the libopencm3 project.
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
 * 
 * Set heap size, stack size and stack location according
 * to application requirements.
 *
 * Set memory bank area and size if external memory is used.
 *
 */

/* Linker script for AT32F403AxG, 1000K flash, 96K RAM. */

/* Define memory regions. */
MEMORY
{
	ram (rwx)		: ORIGIN = 0x20000000, LENGTH = 96K
	rom (rx)		: ORIGIN = 0x08000000, LENGTH = 1000K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

