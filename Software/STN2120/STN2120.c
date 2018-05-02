/*
STN2120.c

Written by Owen Lyke April 2018
Updated May 2018

Header file: STN2120.h
*/


#include "STN2120.h"

// Control functions
void STN2120_reset(STN2120_HandleTypeDef *hSTN2120)
{
	uint8_t str_reset[] = "ATZ";
	STN2120_command(hSTN2120, str_reset);
}

void STN2120_auto_detect_protocol(STN2120_HandleTypeDef *hSTN2120)
{
	uint8_t str_auto_detect[] = "ATSP0";
	STN2120_command(hSTN2120, str_auto_detect);
}



// Data getting functions
uint16_t STN2120_get_mV(STN2120_HandleTypeDef *hSTN2120)
{
	serial_clear(hSTN2120->hserial);
	uint8_t read_v[] = "ATRV";
	STN2120_command(hSTN2120, read_v);
	STN2120_get_response(hSTN2120);
	STN2120_get_response(hSTN2120);

	// Handling this one will be a little different because its not OBD
	STN2120_parse_hex_string(STN2120_shared_data_buffer, hSTN2120->numeral_buffer, 4); 	// Now the numeral buffer should contain 4 bytes, the first two of which are the tens and ones, the last two are tenths and hundredths
	uint16_t mV = 0;
	mV += 10000 * hSTN2120->numeral_buffer[0];
	mV += 1000 * hSTN2120->numeral_buffer[1];
	mV += 100 * hSTN2120->numeral_buffer[2];
	mV += 10 * hSTN2120->numeral_buffer[3];
	return mV;
}

uint8_t STN2120_get_kmh(STN2120_HandleTypeDef *hSTN2120)
{
	serial_clear(hSTN2120->hserial);
	uint8_t kmh[] = "010D";
	STN2120_command(hSTN2120, kmh);
	STN2120_get_response(hSTN2120);
	STN2120_get_response(hSTN2120);
	STN2120_parse_hex_string(STN2120_shared_data_buffer, hSTN2120->numeral_buffer, 4);
	return (uint8_t) hSTN2120->numeral_buffer[2];
}

uint16_t STN2120_get_4rpm(STN2120_HandleTypeDef *hSTN2120)
{
	serial_clear(hSTN2120->hserial);
	uint8_t rpm[] = "010C";
	STN2120_command(hSTN2120, rpm);
	STN2120_get_response(hSTN2120);
	STN2120_get_response(hSTN2120);
	STN2120_parse_hex_string(STN2120_shared_data_buffer, hSTN2120->numeral_buffer, 4);
	return (uint16_t) ((hSTN2120->numeral_buffer[2] << 8) | hSTN2120->numeral_buffer[3]);
}

uint8_t STN2120_get_C_coolant(STN2120_HandleTypeDef *hSTN2120)
{
	serial_clear(hSTN2120->hserial);
	uint8_t C_coolant[] = "0105";
	STN2120_command(hSTN2120, C_coolant);
	STN2120_get_response(hSTN2120);
	STN2120_get_response(hSTN2120);
	STN2120_parse_hex_string(STN2120_shared_data_buffer, hSTN2120->numeral_buffer, 4);
	return (uint8_t) hSTN2120->numeral_buffer[2];
}

uint8_t STN2120_get_C_intake(STN2120_HandleTypeDef *hSTN2120)
{
	serial_clear(hSTN2120->hserial);
	uint8_t C_intake[] = "010F";
	STN2120_command(hSTN2120, C_intake);
	STN2120_get_response(hSTN2120);
	STN2120_get_response(hSTN2120);
	STN2120_parse_hex_string(STN2120_shared_data_buffer, hSTN2120->numeral_buffer, 4);
	return (uint8_t) hSTN2120->numeral_buffer[2];
}





// Utility functions
void STN2120_initialize(STN2120_HandleTypeDef *hSTN2120)
{
	serial_initialize(hSTN2120->hserial);
}

void STN2120_write(STN2120_HandleTypeDef *hSTN2120, uint8_t * pdata, uint8_t size)
{
	serial_write(hSTN2120->hserial, pdata, size);
}

void STN2120_print(STN2120_HandleTypeDef *hSTN2120, uint8_t * pdata)
{
	serial_print(hSTN2120->hserial, pdata);
}

void STN2120_command(STN2120_HandleTypeDef *hSTN2120, uint8_t * pdata)
{
	uint8_t retchar = STN2120_RETURN_CHARACTER;
	STN2120_print(hSTN2120, pdata);
	STN2120_write(hSTN2120, &retchar, 1);
}


void STN2120_get_response(STN2120_HandleTypeDef *hSTN2120)
{
	uint8_t inChar = 0x00;
	uint8_t rxcount = 0;
	while(inChar != STN2120_RETURN_CHARACTER)
	{
		if(hSTN2120->hserial->num_avail > 0)
		{
			serial_read(hSTN2120->hserial, &inChar, 1);	// Read the byte
			STN2120_shared_data_buffer[rxcount++] = inChar;
		}
	}
	// Ensure that there will be a null character in the last index of the shared buffer, at least.
	if(rxcount >= STN2120_DATA_BUFF_NUM_BYTES){ rxcount = STN2120_DATA_BUFF_NUM_BYTES-1; }
	STN2120_shared_data_buffer[rxcount] = '\0';
}

uint8_t STN2120_parse_hex_string(uint8_t * str_buff, uint8_t * num_buff, uint8_t byte_size_max)
{
	uint8_t byte_count = 0;
	uint8_t nibb_count = 0;
	uint8_t str_count = 0;
	for(uint8_t indi = 0; indi < byte_size_max; indi++)
	{
		*(num_buff + indi) = 0;
	}
	while( (*(str_buff + str_count) != '\0') && (byte_count < byte_size_max) )
	{
		switch(*(str_buff + str_count++))
		{
			case '0' : *(num_buff + byte_count) |= (0x0 << (4*(1-nibb_count++))); break;
			case '1' : *(num_buff + byte_count) |= (0x1 << (4*(1-nibb_count++))); break;
			case '2' : *(num_buff + byte_count) |= (0x2 << (4*(1-nibb_count++))); break;
			case '3' : *(num_buff + byte_count) |= (0x3 << (4*(1-nibb_count++))); break;
			case '4' : *(num_buff + byte_count) |= (0x4 << (4*(1-nibb_count++))); break;
			case '5' : *(num_buff + byte_count) |= (0x5 << (4*(1-nibb_count++))); break;
			case '6' : *(num_buff + byte_count) |= (0x6 << (4*(1-nibb_count++))); break;
			case '7' : *(num_buff + byte_count) |= (0x7 << (4*(1-nibb_count++))); break;
			case '8' : *(num_buff + byte_count) |= (0x8 << (4*(1-nibb_count++))); break;
			case '9' : *(num_buff + byte_count) |= (0x9 << (4*(1-nibb_count++))); break;
			case 'A' : *(num_buff + byte_count) |= (0xA << (4*(1-nibb_count++))); break;
			case 'B' : *(num_buff + byte_count) |= (0xB << (4*(1-nibb_count++))); break;
			case 'C' : *(num_buff + byte_count) |= (0xC << (4*(1-nibb_count++))); break;
			case 'D' : *(num_buff + byte_count) |= (0xD << (4*(1-nibb_count++))); break;
			case 'E' : *(num_buff + byte_count) |= (0xE << (4*(1-nibb_count++))); break;
			case 'F' : *(num_buff + byte_count) |= (0xF << (4*(1-nibb_count++))); break;

			default : break;
		}
		if(nibb_count > 1)
		{
			nibb_count = 0;
			byte_count++;
		}
	}
	return byte_count;
}
