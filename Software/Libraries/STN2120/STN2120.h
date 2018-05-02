/*
STN2120.h

Written by Owen Lyke April 2018
Updated May 2018

Used to provide OBD-II interface functions via the STN2120 OBD interpreter IC
*/

#ifndef STN2120_H
#define STN2120_H

#include "stm32f4xx_hal.h"
#include "serial.h"

#define STN2120_RETURN_CHARACTER '\r' 	// Should be equivalent to 0x0D
#define STN2120_DATA_BUFF_NUM_BYTES	40	//

uint8_t STN2120_shared_data_buffer[STN2120_DATA_BUFF_NUM_BYTES];
/* This buffer is shared by all instances of the STN2120. Won't be a problem if you have only 1 instance,
 * and if you have more it shouldn't be a problem because of the blocking operating principal. Just don't
 * keep your data here for extended periods!
*/



typedef enum{
	STN2120_SEARCHING = 0,

}STN2120_StatusTypeDef;



typedef struct{
	uint8_t 				SX; // Printing spaces on (1) or off (0)
	uint8_t					EX; // Character echo on(1) or off(0)
}STN2120_SettingsTypeDef;


typedef struct{
	serial_HandleTypeDef	*hserial;			// The serial object to which the STN2120 is connected
	uint8_t					numeral_buffer[4];	// This is a 4 byte wide variable used to store parsed OBD responses, not really meant to be used by the user
	uint8_t					status;				// Used to store the status of the latest command
}STN2120_HandleTypeDef;



// Functions
// Control functions
void STN2120_reset(STN2120_HandleTypeDef *hSTN2120);								// Returns
void STN2120_auto_detect_protocol(STN2120_HandleTypeDef *hSTN2120);


// Data getting functions
uint16_t STN2120_get_mV(STN2120_HandleTypeDef *hSTN2120);							// Gets measured battery voltage in millivolts
uint8_t STN2120_get_kmh(STN2120_HandleTypeDef *hSTN2120);							// Gets vehicle speed in kilometers per hour
uint16_t STN2120_get_4rpm(STN2120_HandleTypeDef *hSTN2120);							// Gets tachometer reading multiplied by four (aka in quarter-rpm)
uint8_t STN2120_get_C_coolant(STN2120_HandleTypeDef *hSTN2120);					// Gets the coolant temperature in degrees C
uint8_t STN2120_get_C_intake(STN2120_HandleTypeDef *hSTN2120);						// Gets the intake air temperature in degrees C


// Utility functions
void STN2120_initialize(STN2120_HandleTypeDef *hSTN2120);
void STN2120_write(STN2120_HandleTypeDef *hSTN2120, uint8_t * pdata, uint8_t size);									// Writes bytes directly to the STN2120, no formatting applied
void STN2120_command(STN2120_HandleTypeDef *hSTN2120, uint8_t * pdata);											// Writes the data in pdata to the STN2120 until a null character is reached and then sends a carriage return character 0x0D to terminate the command
void STN2120_get_response(STN2120_HandleTypeDef *hSTN2120);
uint8_t STN2120_parse_hex_string(uint8_t * str_buff, uint8_t * num_buff, uint8_t size_max);

#endif /* STN2120_H */


