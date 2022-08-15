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

#if !defined SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/usb/cdc.h>

#include "system.h"


#define RX_DMA_SIZE					256
#define TX_DMA_SIZE 				256

#define SERIAL_RING_BUFFER_SIZE_POWER 10
#define SERIAL_RING_BUFFER_SIZE (1 << SERIAL_RING_BUFFER_SIZE_POWER)

struct sring
{
	uint8_t *data;
	uint16_t put_ptr;
	uint16_t get_ptr;
};

#define X_ON								17
#define X_OFF 							19
#define X_OFF_TRIGGER				(3 * SERIAL_RING_BUFFER_SIZE / 4)
#define X_ON_TRIGGER				(SERIAL_RING_BUFFER_SIZE / 2)

//#define USBUSART_TDR        USART_DR(USART_PORT)
//#define USBUSART_RDR        USART_DR(USART_PORT)

#if	USART_PORT == USART1
#define RCC_USART						RCC_USART1
#define ISR_USART						void usart1_isr(void)
#define 
#if MCU == STM32F103
#define RCC_DMA							RCC_DMA1
#define USART_DMA_BUS				DMA1
#define USART_DMA_TX_CH			DMA_CHANNEL4
#define USART_DMA_TX_IRQ		NVIC_DMA1_CHANNEL4_IRQ
#define ISR_DMA_CH_USART_TX	void dma1_channel4_isr(void)
#define USART_DMA_RX_CH			DMA_CHANNEL5
#define USART_DMA_RX_IRQ		NVIC_DMA1_CHANNEL5_IRQ
#define ISR_DMA_CH_USART_RX	void dma1_channel5_isr(void)
#endif	//#if MCU == STM32F103
#if MCU ==STM32F401
#define RCC_DMA							RCC_DMA2
#define USART_DMA_BUS				DMA2
#define USART_DMA_TX_CH			DMA_STREAM7
#define USART_DMA_TX_IRQ		NVIC_DMA2_CHANNEL7_IRQ
#define ISR_DMA_CH_USART_TX	void dma2_channel7_isr(void)
#define USART_DMA_RX_CH			DMA_STREAM5
#define USART_DMA_RX_IRQ		NVIC_DMA2_CHANNEL5_IRQ
#define ISR_DMA_CH_USART_RX	void dma2_channel5_isr(void)
#define USART_DMA_TRG_CHSEL	DMA_SxCR_CHSEL_4	//The same for USART 1 & 2
#endif	//#if MCU ==STM32F401
#define NVIC_USART_IRQ			NVIC_USART1_IRQ
#define GPIO_BANK_USART_TX	GPIO_BANK_USART1_TX
#define GPIO_PIN_USART_TX		GPIO_USART1_TX
#define GPIO_BANK_USART_RX	GPIO_BANK_USART1_RX
#define GPIO_PIN_USART_RX	GPIO_USART1_RX
#endif	//#if	USART_PORT == USART1

#if	USART_PORT == USART2
#define RCC_USART						RCC_USART2
#define ISR_USART						void usart2_isr(void)
#if MCU == STM32F103
#define RCC_DMA							RCC_DMA1
#define USART_DMA_BUS				DMA1
#define USART_DMA_TX_CH			DMA_CHANNEL7
#define USART_DMA_TX_IRQ		NVIC_DMA1_CHANNEL7_IRQ
#define ISR_DMA_CH_USART_TX	void dma1_channel7_isr(void)
#define USART_DMA_RX_CH			DMA_CHANNEL6
#define USART_DMA_RX_IRQ		NVIC_DMA1_CHANNEL6_IRQ
#define ISR_DMA_CH_USART_RX	void dma1_channel6_isr(void)
#endif	//#if MCU == STM32F103
#if MCU ==STM32F401
#define RCC_DMA							RCC_DMA1
#define USART_DMA_BUS				DMA1
#define USART_DMA_TX_CH			DMA_STREAM6
#define USART_DMA_TX_IRQ		NVIC_DMA1_CHANNEL6_IRQ
#define ISR_DMA_CH_USART_TX	void dma1_channel6_isr(void)
#define USART_DMA_RX_CH			DMA_STREAM5
#define USART_DMA_RX_IRQ		NVIC_DMA1_CHANNEL5_IRQ
#define ISR_DMA_CH_USART_RX	void dma1_channel5_isr(void)
#define USART_DMA_TRG_CHSEL	DMA_SxCR_CHSEL_4	//The same for USART 1 & 2
#endif	//#if MCU ==STM32F401
#define NVIC_USART_IRQ			NVIC_USART2_IRQ
#define GPIO_BANK_USART_TX	GPIO_BANK_USART2_TX
#define GPIO_PIN_USART_TX		GPIO_USART2_TX
#define GPIO_BANK_USART_RX	GPIO_BANK_USART2_RX
#define GPIO_PIN_USART_RX		GPIO_USART2_RX
#endif	//#if	USART_PORT == USART2


void serial_setup(void);

void usart_update_comm_param(struct usb_cdc_line_coding*);

// Put a char (uint8_t) on serial buffer.
// They return number of chars are in the buffer or 0xFFFF when there was no room to add this char.
// They are non blocking functions
uint16_t con_put_char(uint8_t);
uint16_t ring_put_ch(struct sring*, uint8_t);

// Put a char (uint8_t) on uart_tx_ring, but send to serial buffer if there is avaiable room.
// It returns the number os chars on uart_tx_ring.
uint16_t uart_tx_ring_dma_send_ch(uint8_t);

// Send a ASCIIZ string to serial (up to 127 chars).
// It is a non blocking function if there is room on TX Buffer
void con_send_string(uint8_t*);

// If there is an available char in USART2 RX ring, it return true.
// They are non blocking functions
uint16_t ring_avail_get_ch(struct sring*);
uint16_t con_available_get_char(void);

// If there is an available char in serial, it returns with an uint8_t.
// They are non blocking functions
uint8_t ring_get_ch(struct sring*, uint16_t*);
uint8_t con_get_char(void);

// Read a line from serial. You can limit how many char will be available to enter.
// It returns how many chars were read.
// It is a blocking function
uint8_t console_get_line(uint8_t*, uint16_t);

/* To be used with printf */
int _write(int, char*, int);

/*Functions to convert strings*/
// Convert a word (32 bit) into a up to 8 char string.
void conv_uint32_to_dec(uint32_t value, uint8_t *outstring);

// Convert a two byte string pointed by i into a binary byte. 
uint8_t conv_2a_hex_to_uint8(uint8_t*, int16_t);

// Convert a word (32 bit binary) to into a 8 char string. 
void conv_uint32_to_8a_hex(uint32_t, uint8_t*);

// Convert a half-word (16 bit binary) to into a 4 char string. 
void conv_uint16_to_4a_hex(uint16_t, uint8_t*);

// Convert a byte (8 bit binary) to into a 2 char string. 
void conv_uint8_to_2a_hex(uint8_t value, uint8_t *outstring);

// Convert a half-word (16 bit binary) to into a 5 char dec string. 
void conv_uint16_to_dec(uint16_t, uint8_t*);


#ifdef __cplusplus
}
#endif

#endif	//#ifndef SERIAL_H
