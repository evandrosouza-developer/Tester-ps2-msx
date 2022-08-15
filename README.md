# STM32: PS/2 to MSX Converter Tester

This code was made to fit to a STM32F103C6T6 (Flash 32K RAM 10K), so this code may be applyed to a STM32F103C8T6 (Flash 64K RAM 20K) without changes and it was done to facilitate a Blue Pill to act as a MSX keyboard sub system emulator, to test (and develop) the PS/2 keyboard to MSX adapter.

This updated version has Serial now running over DMA, thus freeing processor processing power, and so, let resources to implement USB.

When there is no USB host, it works at legacy (using serial 2 as console port), but when its USB is enumerated, the console port is the first logical cdcacm port on host, and the second logical port is a serial<=>USB converter. It is possible to adjust serial speed, 8 or 9 data bits, parity(none/even/odd) and stop bits

This code is common to the two adapters I made, both based in STM32 and fits to this chip. The flash used by this implementation fits with available amount:

 ```
arm-none-eabi-size tester-ps2-max.elf

text  data bss   dec  hex  filename
22800 1136 6196 30132 75b4 tester-ps2-msx.elf`
```
 

***************  IMPORTANT NOTES **********************************************
1) The system is configurable through a 3.3V serial console, so DO NOT use 5V or RS-232 voltage levels here, as they are going to instantly damage your board!
2) 3.3V is compatible with standard TTL Levels.
*******************************************************************************


## Boot screen:
```
MSX keyboard subsystem emulator based on STM32F103
Serial number is XXXXXXXX
Firmware built on MM dd yyyy hh:mm:ss

Booting...

. Auxiliary message. Refer to following details.

Configuring:
- 5V compatible pin ports and interrupts to interface like a real MSX;
- High resolution timer2;
- SysTick;
- Ports config locking.

Boot complete! Press ? to show available options.

> 
```


Details about boot message:

=>If usb is enumerated, the message will be:

. USB has been enumerated => Console and UART are over USB.

=>If usb was not enumerated, the message will be:

. USB host not found => Using Console over USART.

=>If the device does not support usb, the message will be:

. Non USB version. Console is over USART.


## Options menu:
```
(?) Available options
1) General:
   r (Show Running config);
   c (Caps Lock line <- On/Off/Blink);
   k (Kana line      <- On/Off/Blink);
2) Scan related:
   s (Scan submenu - Set first [Y Begin] and last [Y End] colunms to scan);
   + (Increase scan rate);
   - (Decrease scan rate);
   p (Toggle pause scan);
   n (Next step colunm scan)                        <= when scan is paused;
   Space (One shot scan, from [Y Begin] to [Y End]) <= when scan is paused;
3) Times / Delays / Duties:`   a) Time to read X_Scan (after Y_Scan) update:
   < (decrease by 0.25μs);
   > (increase by 0.25μs);
   b) Read duty cycle: 1 work N idle. N may be 0 to maximum for speed:
   i (After one sweep active read cycle, configure number of idle cycles).
> 
```

## Dependencies

- `arm-none-eabi-gcc`
- `arm-none-eabi-gdb`
- `arm-none-eabi-binutils`
- `Black Magic Probe`
- `stlink` + `openocd (if you want to debug)`

## Preparations

After cloning the repository you need to make the following preparations:

```
git submodule init
git submodule update
cd libopencm3
make
cd ..
make
```

## Hardware and Setup

You will obviously need a STM32F103C6T6 or a STM32F103C8T6 chip. I have used a chinese blue pill. The software was made aiming in use of compatible processors, like GD32 for example. The software was made considering 8.000Mhz oscillator crystal, to clock the STM32 microcontroller chip at 72MHz. The connections are:

1) Serial console:

Config: 115200, 8, n, 1 (115200 bps, 8 bits, no parity, 1 stop bit;

Tx: A2

Rx: A3

*******************************************************************************************************

Obs.: It is a only 3.3V port, compatible to TTL levels. Do not use it with "1" level higher than 3.3V!!

*******************************************************************************************************


2) To PS/2 to MSX Adapter:
- PB8  (X0) - Connect to /X0 pin of the adapter;
- PB9  (X1) - Connect to /X1 pin of the adapter;
- PB10 (X2) - Connect to /X2 pin of the adapter;
- PB11 (X3) - Connect to /X3 pin of the adapter;
- PB12 (X4) - Connect to /X4 pin of the adapter;
- PB13 (X5) - Connect to /X5 pin of the adapter;
- PB14 (X6) - Connect to /X6 pin of the adapter;
- PB15 (X7) - Connect to /X7 pin of the adapter;
- PA4  (Y0) - Connect to Y0 pin of the adapter;
- PA5  (Y1) - Connect to Y1 pin of the adapter;
- PA6  (Y2) - Connect to Y2 pin of the adapter;
- PA7  (Y3) - Connect to Y3 pin of the adapter;
- PB5 (CAPS)- Connect to /Caps pin of the adapter;
- PB6 (KANA)- Connect to /Kana pin of the adapter; pull-up connection.


## Hardware observations

No PCB will be developed for this tester, as I recommend the aquisition of blue pill for this function.

Use a ST-Link v2 Programmer (or similar), Black Magic Probe or another Serial Wire supported tool to flash the program using `make flash` onto the STM32.
