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

#ifndef msxmap_h
#define msxmap_h

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

//Use Tab width=2


class msxmap
{
private:

public:
	void msx_interface_setup(void);
	void general_debug_setup(void);
};

	void portXread(void);

#endif
