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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef T_SYSTEM_H
#define T_SYSTEM_H


#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/usb/usbd.h>


/* Microcontroller STM32F103 or STM32F401 */
#define	STM32F103									0x410			//Blue Pill
#define	STM32F401									0x423			//WeAct Black Pill

#define	MCU												STM32F103

// Place to get the microcontroller unique id to compute serial number
#ifndef DESIG_UNIQ_ID_BASE
#if MCU == STM32F103
#define	DESIG_UNIQ_ID_BASE				0x1FFFF7E8;
#define	LEN_SERIAL_No							8
#endif	//#if MCU == STM32F103
#if MCU ==STM32F401
#define	DESIG_UNIQ_ID_BASE				0x1FFF7A10;
#define	LEN_SERIAL_No							12
#endif	//#if MCU ==STM32F401
#endif	//#ifndef DESIG_UNIQ_ID_BASE

/* High Resolution Timer definitions */
#define TIM_HR										TIM2				//Here we define which timer we will use for hrtimer
#if TIM_HR == TIM2
#define RCC_TIM_HR								RCC_TIM2
#define RST_TIM_HR								RST_TIM2
#define NVIC_TIM_HR_IRQ						NVIC_TIM2_IRQ
#define ISR_TIM_HR								void tim2_isr(void)
#endif	//#if TIM_HR_TIMER == TIM2

/* Interrupt priority definitions. From 0 to 15. Low numbers are high priority. */
#define IRQ_PRI_TIM_HR						(1 << 4)
#define IRQ_PRI_SYSTICK						(2 << 4)
#define IRQ_PRI_USB								(3 << 4)
#define IRQ_PRI_USART							(5 << 4)
#define IRQ_PRI_USART_DMA					(5 << 4)

/* General definitions about MSX keyboard scan control */
#define SCAN_POINTER_SIZE					18
#define DELAY_TO_READ_SIZE				14
#define INIT_SCAN_POINTER					15
#define INIT_DELAY_TO_READ_X_SCAN	6

/* Hardware port definitions */
#if MCU == STM32F103
#define USART_PORT USART2

#define EMBEDDED_LED_port					GPIOC
#define EMBEDDED_LED_pin					GPIO13
#define X7_port										GPIOB
#define X7_pin_id									GPIO15
#define X7_exti										EXTI15
#define X6_port										GPIOB
#define X6_pin_id									GPIO14
#define X6_exti										EXTI14
#define X5_port										GPIOB
#define X5_pin_id									GPIO13
#define X5_exti										EXTI13
#define X4_port										GPIOB
#define X4_pin_id									GPIO12
#define X4_exti										EXTI12
#define X3_port										GPIOB
#define X3_pin_id									GPIO9
#define X3_exti										EXTI9
#define X2_port										GPIOB
#define X2_pin_id									GPIO8
#define X2_exti										EXTI8
#define X1_port										GPIOB
#define X1_pin_id									GPIO7
#define X1_exti										EXTI7
#define X0_port										GPIOB
#define X0_pin_id									GPIO6
#define X0_exti										EXTI6
#define Y_port										GPIOA
#define Y_pin_id									GPIO0
#define Y3_port										GPIOA
#define Y3_pin_id									GPIO7
#define Y2_port										GPIOA
#define Y2_pin_id									GPIO6
#define Y1_port										GPIOA
#define Y1_pin_id									GPIO5
#define Y0_port										GPIOA
#define Y0_pin_id									GPIO4
#define CAPS_port									GPIOB
#define CAPS_pin_id								GPIO4
#define KANA_port									GPIOB
#define KANA_pin_id								GPIO5
//SERIAL1_port										GPIOA (Pre-defined)
//SERIAL1_TX_pin_id								GPIO9 (Pre-defined)
//SERIAL1_RX_pin_id								GPIO10(Pre-defined)
//SERIAL2_port										GPIOA (Pre-defined)
//SERIAL2_TX_pin_id								GPIO2 (Pre-defined)
//SERIAL2_RX_pin_id								GPIO3 (Pre-defined)
//SERIAL3_port										GPIOB (Pre-defined)
//SERIAL3_TX_pin_id								GPIO10(Pre-defined)
//SERIAL3_RX_pin_id								GPIO11(Pre-defined)


//Force Y_port_id pin (Sync pin) to high, so the first time slot is a low => Falling transition on the start of frame
//Force Xint_pin_id to low. portXread in msxmap.cpp will put this in high at each port update: to be possible mesaure real time of reading.
#define Y_0												0x00F20001  // 15859713
#define Y_1												0x00E20011  // 14811153
#define Y_2												0x00D20021  // 13762593
#define Y_3												0x00C20031  // 12714033
#define Y_4												0x00B20041  // 11665473
#define Y_5												0x00A20051  // 10616913
#define Y_6												0x00920061  // 9568353
#define Y_7												0x00820071  // 8519793
#define Y_8												0x00720081  // 7471233
#define Y_9												0x00620091  // 6422673
#define Y_A												0x005200A1  // 5374113
#define Y_B												0x004200B1  // 4325553
#define Y_C												0x003200C1  // 3276993
#define Y_D												0x002200D1  // 2228433
#define Y_E												0x001200E1  // 1179873
#define Y_F												0x000200F1  // 131313


//To debug
#define Xint_port									GPIOA
#define Xint_pin_id								GPIO1
#define INT_TIM2_port							GPIOB
#define TIM2UIF_pin_id						GPIO0
#define SYSTICK_port							GPIOB
#define SYSTICK_pin_id						GPIO1

#endif	//#if MCU == STM32F103


/* USB related definitions */

/* Define the usage of USB */
#define	USE_USB										true

#define CDC_ONLY_ON_USB						true			// If we are providing serial interfaces only => True

#if USE_USB == true
#if MCU == STM32F103
#define USB_DRIVER     						st_usbfs_v1_usb_driver
#define USB_ISR										void usb_lp_can_rx0_isr(void)
#define USB_NVIC									NVIC_USB_LP_CAN_RX0_IRQ
#endif	//#if MCU == STM32F103C8
#if MCU ==STM32F401
#define USB_DRIVER      					stm32f107_usb_driver
#define USB_ISR										void otg_fs_isr(void)
#define USB_NVIC									NVIC_OTG_FS_IRQ
#endif	//#if MCU == STM32F401CC

#define OTG_DCTL									0x804
#define OTG_FS_DCTL								MMIO32(USB_OTG_FS_BASE + OTG_DCTL)
#define OTG_FS_DCTL_SDIS					1<<1				//0: Normal operation. 2: The core generates a device disconnect event to the USB host.
#define OTG_FS_DSTS								MMIO32(USB_OTG_FS_BASE + OTG_DSTS)
#define OTG_FS_DSTS_SUSPSTS				1<<0				//0: Suspend condition is detected on the USB. 1: Normal operation.

#define USB_CLASS_MISCELLANEOUS 	0xEF  //  Idea taked from Blue Pill Bootloader

#ifndef USB_VID
//#define	USB_VID									0x0483			//ST VID
#define	USB_PID										0x5740			//ST CDC from various forums
#define	USB_VID										0x1d50			//Black magic Probe
//#define	USB_PID									0x6018			//Black magic Probe
//#define	USB_VID									0x0ACE			//ZyXEL Omni FaxModem 56K Plus
//#define	USB_PID									0x1611			//ZyXEL Omni FaxModem 56K Plus
//#define	USB_VID									0x0A05			//Same VID as my USB hub			
//#define	USB_PID									0x5740			//Ex.: My hub is 0x7211
#endif	//	#ifndef USB_VID

#endif	//#if USE_USB == true

/*  Index of each USB interface. Must be consecutive and must sync with interfaces[].*/
enum INTF{
	INTF_CON_COMM =									0,
	INTF_CON_DATA,
	INTF_UART_COMM,
	INTF_UART_DATA,
};

//  USB Endpoints addresses. Starts with 1, as endpoint 0 is the default.
enum ENDPOINT{
	EP_CON_DATA_OUT	=								1,					//CDC Data OUT of FIRST endpoint
	EP_C1,																			//CDC Command of FIRST endpoint: uses this as +0x80
	EP_UART_DATA_OUT,														//CDC Data OUT of SECOND endpoint
	EP_C2,																			//CDC Command of SECOND endpoint: uses this as +0x80
	EP_CON_DATA_IN	=	EP_CON_DATA_OUT | 0x80,		//CDC Data IN of First endpoint. (0x80=USB_REQ_TYPE_IN)
	EP_CON_COMM,																//First endpoint: valid add CDC Command 
	EP_UART_DATA_IN,														//CDC Data IN of Second endpoint
	EP_UART_COMM,																//Second endpoint: CDC Command
};

#define MAX_USB_PACKET_SIZE				64
#define COMM_PACKET_SIZE					MAX_USB_PACKET_SIZE / 4
#define USBD_DATA_BUFFER_SIZE			MAX_USB_PACKET_SIZE

//#define USB21_INTERFACE					true				//  Enable USB 2.1 with WebUSB and BOS support.
#define USB21_INTERFACE						false				//  Disable USB 2.1 with WebUSB and BOS support.



#endif	//#ifndef T_SYSTEM_H

#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus
