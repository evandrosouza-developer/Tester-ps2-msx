##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

## Directories:
SRC =
OBJ =
DEST_FILES =

BINARY = tester-ps2-msx
OBJS = msxmap.o sys_timer.o serial_no.o cdcacm.o serial.o hr_timer.o

LDSCRIPT = stm32f103x6.ld

include libopencm3.target.mk

## Para chamar o openocd, use:
## sudo openocd -s ../tcl -f stlink-swd.ocd
## ou o st-link

## Para executar o gdb, use:
## 1) Se o server for o openocd:
## arm-none-eabi-gdb target extended-remote localhost:3333
##
## 2) Se o server for o st-link:
## arm-none-eabi-gdb target extended-remote localhost:4242
