/** @defgroup USART with DMA peripheral API
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
 * This library supports the USART with DMA in the STM32F4 and STM32F1
 * series of ARM Cortex Microcontrollers by ST Microelectronics.
 *
 * LGPL License Terms @ref lgpl_license
 */

/*
 * This file is part of the PS/2 to MSX keyboard Converter Enviroment,
 * covering MSX keyboard Converter and MSX Keyboard Subsystem Emulator
 * designs, based on libopencm3 project.
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

/**@{*/

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


struct sring
{
  uint8_t *data;
  uint16_t bufSzMask;
  uint16_t put_ptr;
  uint16_t get_ptr;
};

#define X_ON                17
#define X_OFF               19


/** @brief Setup the USART and DMA
 * @param none
 * @return none
*/
void serial_setup(void);


/** @brief Restart the USART
 * @param none
 * @return none
*/
void serial_rx_restart(void);


/**
 * @brief Puts the struct usb_cdc_line_coding parameters onto serial.
 * @param pointer to struct usb_cdc_line_coding.
 * @return none
 */
void usart_update_comm_param(struct usb_cdc_line_coding*);


/**
 * @brief Does prepare DMA if it is idle if DMA is idle. It is used force a start DMA sending of uart_tx_ring, to let DMA routines take control until buffer is flushed.
 * @param pointer to struct usb_cdc_line_coding.
 * @return none
 */
void do_dma_usart_tx_ring(uint16_t number_of_data);

/**
 * @brief Inits the struct sring ring.
 * @param pointer to struct sring.
 * @param pointer to already defined buffer.
 * @param size of the already defined buffer.
 * @return none
 */
void ring_init(struct sring*, uint8_t*, uint16_t);


/**
 * @brief Inits the s_pascal_string.
 * @param pointer to s_pascal_string.
 * @param pointer to already defined buffer.
 * @param size of the already defined buffer.
 * @return none
 */
void pascal_string_init(struct s_pascal_string*, uint8_t*, uint8_t);


/**
 * @brief Appends an ASCIIZ (uint8_t) string at the end of s_pascal_string buffer.
 * @param pointer to ASCIIz string to to struct sring+
 * @param pointer to struct sring.
 * @return none
 */
void string_append(uint8_t*, struct s_pascal_string*);


/**
 * @brief Puts a byte in the specified ring. It is a non blocking function.
 * @param pointer to struct sring.
 * @param byte to put.
 * @return the effective number of bytes available in the specified ring.
 */
uint16_t ring_put_ch(struct sring*, uint8_t);


/**
 * @brief Send a ASCIIZ string to serial (up to 127 chars) to console buffer and starts sending. It is a non blocking function while there is room on TX Buffer.
 * @param pointer to string to send via console.
 * @return none
 */
void con_send_string(uint8_t*);


/**
 * @brief It returns the number of availabe bytes in the specified ring. It is a non blocking function
 * @param pointer to struct sring.
 * @return number of availabe bytes are available in the specified ring.
 */
uint16_t ring_avail_get_ch(struct sring*);


/**
 * @brief Used to verify the availability in the actual console buffer. It is a non blocking function
 * @param none.
 * @return the effective number of bytes are available in the actual console buffer.
 */
uint16_t con_available_get_char(void);


// If there is an available char in serial, it returns with an uint8_t, but
// before, you have to use con_available_get_char or ring_avail_get_ch to check their availability.
// They are non blocking functions.
/**
 * @brief If there is an available char in serial, it returns with an uint8_t. It is a non blocking function
 * @param pointer to struct sring.
 * @param pointer of how many bytes are available to read in the specified ring.
 * @return the effective number of bytes
 */
uint8_t ring_get_ch(struct sring*, uint16_t*);


/**
 * @brief If there is an available char in console ring, it returns with an uint8_t. It is a non blocking function
 * @param none.
 * @return the effective number of bytes
 */
uint8_t con_get_char(void);


/**
 * @brief Read a line from console. It is a blocking function.
 * @param Pointer with the address to put reading.
 * @param Maximum number of chars to read.
 * @return How many chars were read.
 */
uint8_t console_get_line(uint8_t*, uint16_t);


//Force next console reading ch
/**
 * @brief Forces console next reading ch. It is assumed that actual console buffer is empty.
 * @param char to buf buffer to copy data to
 * @return none
 */
void insert_in_con_rx(uint8_t);


/* To be used with printf */
/**
 * @brief To be used with printf.
 * @param file number
 * @param pointer of char with the address of the string
 * @return the effective number of bytes in the string or -1 when error.
 */
int _write(int, char*, int);


/*Functions to convert strings*/

/**
 * @brief Convert a word (32 bit) into a up to 8 char string.
 * @param word (32 bit binary)
 * @param address to where put the stringz result
 * @return none
 */
void conv_uint32_to_dec(uint32_t, uint8_t*);


/**
 * @brief Convert a two byte string pointed by i into a binary byte. 
 * @param address of the string.
 * @param index of first byte (high nibble) into the string to be converted.
 * @return byte with the convertion.
 */
uint8_t conv_2a_hex_to_uint8(uint8_t*, int16_t);


/**
 * @brief Convert a word (32 bit binary) to into a 8 char string. 
 * @param word (32 bit binary)
 * @param address to where put the stringz result
 * @return none
 */
void conv_uint32_to_8a_hex(uint32_t, uint8_t*);


/**
 * @brief Convert a half-word (16 bit binary) to into a 4 char string. 
 * @param half-word (16 bit binary) number to be converted
 * @param address to where put the stringz result
 * @return none
 */
void conv_uint16_to_4a_hex(uint16_t, uint8_t*);


/**
 * @brief Convert a byte (8 bit binary) to into a 2 char string. .
 * @param byte (8 bit binary) number to be converted
 * @param address to where put the stringz result
 * @return none
 */
void conv_uint8_to_2a_hex(uint8_t, uint8_t*);


/**
 * @brief Convert a half-word (16 bit binary) to into a 5 char dec string. 
 * @param half-word (16 bit binary) number to be converted
 * @param address to where put the stringz result
 * @return none
 */
void conv_uint16_to_dec(uint16_t, uint8_t*);



#ifdef __cplusplus
}
#endif

#endif  //#ifndef SERIAL_H
