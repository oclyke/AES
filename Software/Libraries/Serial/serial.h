/* 
serial.h

Written by Owen Lyke April 2018
Updated April 2018

Purpose is to provide a simple serial interface on the STM32F411, akin to Arduino serial library

To use this library the user should specify the uart settings they need (in huart.Init) and then call the serial_initialize function
*/

#ifndef SERIAL_H
#define SERIAL_H

#include "stm32f4xx_hal.h"

#define serial_default_receive_size 1
#define serial_default_buff_size 256



uint8_t serial_buffer_USART1[serial_default_buff_size];
uint8_t serial_buffer_USART2[serial_default_buff_size];
uint8_t serial_buffer_USART6[serial_default_buff_size];

typedef struct{
	UART_HandleTypeDef 		*huart; 	// Associates a hardware uart with the serial "object"

	volatile uint8_t 		lock;		// A temporary storage place for a single byte while it gets handled properly: like a "lock" for boats
	uint8_t					*pbuff;		// Pointer to the buffer to be used for the communication

	volatile uint8_t					transmit_complete;	// Value of 1 if the transmit is complete, zero otherwise

	uint8_t					active;		// Indicates if the serial object is being used
	volatile uint16_t		write_index;// The index of pbuff at which to store the next received byte, if allowed
	volatile uint16_t		read_index;	// The index of pbuff from which to take the next byte
	volatile uint8_t 		write_ok;	// Logical, expresses whether it is OK or not to accept the next received byte
	volatile uint16_t		num_avail;	// The number of unread bytes in the buffer, ready to be read

}serial_HandleTypeDef;

// I decree that the user shall not need to declare their own serial_HandleTypeDef variables, they can use one of these three. Others are not supported. It is assumed that serial1 links with buffer1 and UART1
serial_HandleTypeDef serial1;
serial_HandleTypeDef serial2;
serial_HandleTypeDef serial6;


/* Functions */
void serial_initialize(serial_HandleTypeDef * hserial);
void serial_clear(serial_HandleTypeDef * hserial);												// Resets the serial port - sets writeOK to 1, sets num_avail to 0, sets read and write indices to 0
uint16_t serial_read(serial_HandleTypeDef * hserial, uint8_t * pdata, uint16_t size);
uint8_t serial_peek(serial_HandleTypeDef * hserial);
void serial_write(serial_HandleTypeDef * hserial, uint8_t * pdata, uint16_t size);
void serial_print(serial_HandleTypeDef * hserial, uint8_t * pdata);
void serial_println(serial_HandleTypeDef * hserial, uint8_t * pdata);
void serial_print_uint32(serial_HandleTypeDef * hserial, uint32_t val, uint8_t format, uint8_t min_digits);
void serial_print_double(serial_HandleTypeDef * hserial, double val);

void serial_HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);
void serial_HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart);

uint32_t serial_utility_DD(uint16_t bin);							// Double dabble makes uints into BPD






#endif /* SERIAL_H */


