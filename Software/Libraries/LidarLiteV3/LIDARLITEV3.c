/*

Owen Lyke
April 2018

Header File: "lidarlitev3.h"

A library to interface with the Lidarlite V3 Lidar Rangefinder

*/

#include "LIDARLITEV3.h"


void LIDARLITEV3_init(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3)
{
	LIDARLITEV3_assert_i2c_bus(hLIDARLITEV3);																													// Ensure the I2C bus is ready for this sensor

	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_SIG_COUNT_VAL, 		&(hLIDARLITEV3->Init.SIG_COUNT_VAL_VAL), 		1, hLIDARLITEV3->Timeout); 				// Set SIG_COUNT_VAL from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_ACQ_CONFIG_REG, 	&(hLIDARLITEV3->Init.ACQ_CONFIG_REG_VAL), 		1, hLIDARLITEV3->Timeout); 				// Set ACQ_CONFIG_REG from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_THRESHOLD_BYPASS, 	&(hLIDARLITEV3->Init.THRESHOLD_BYPASS_VAL), 	1, hLIDARLITEV3->Timeout); 				// Set THRESHOLD_BYPASS from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_OUTER_LOOP_COUNT, 	&(hLIDARLITEV3->Init.OUTER_LOOP_COUNT_VAL), 	1, hLIDARLITEV3->Timeout); 				// Set OUTER_LOOP_COUNT from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_REF_COUNT_VAL, 		&(hLIDARLITEV3->Init.REF_COUNT_VAL_VAL), 		1, hLIDARLITEV3->Timeout); 				// Set REF_COUNT_VAL from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_COMMAND, 			&(hLIDARLITEV3->Init.COMMAND_VAL), 				1, hLIDARLITEV3->Timeout); 				// Set COMMAND from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_MEASURE_DELAY, 		&(hLIDARLITEV3->Init.MEASURE_DELAY_VAL), 		1, hLIDARLITEV3->Timeout); 				// Set MEASURE_DELAY from user values
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_POWER_CONTROL, 		&(hLIDARLITEV3->Init.POWER_CONTROL_VAL), 		1, hLIDARLITEV3->Timeout); 				// Set POWER_CONTROL from user values
}

void 	LIDARLITEV3_init_continuous_measurement(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t DELAY, uint8_t MEASURE_TYPE)
{
	hLIDARLITEV3->Init.MEASURE_DELAY_VAL = DELAY;
	hLIDARLITEV3->Init.ACQ_CONFIG_REG_VAL |= LIDARLITEV3_USE_MEASURE_DELAY;
	hLIDARLITEV3->Init.OUTER_LOOP_COUNT_VAL = 0xFF;

	LIDARLITEV3_init(hLIDARLITEV3);

	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_ACQ_COMMAND, &MEASURE_TYPE, 1, hLIDARLITEV3->Timeout);
}

void	LIDARLITEV3_reset(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3)
{
	uint8_t MEASURE_TYPE = LIDARLITEV3_RESET;
	LIDARLITEV3_write(hLIDARLITEV3, LIDARLITEV3_REG_ACQ_COMMAND, &MEASURE_TYPE, 1, hLIDARLITEV3->Timeout);
}






void 	LIDARLITEV3_read(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout)
{
	uint16_t write_address = (((hLIDARLITEV3->i2c_addr_7)<<1) | 0x00);
	uint16_t read_address = (((hLIDARLITEV3->i2c_addr_7)<<1) | 0x01);
	HAL_I2C_Master_Transmit(hLIDARLITEV3->hi2c, write_address, &reg_add, 1, timeout);
	HAL_I2C_Master_Receive(hLIDARLITEV3->hi2c, read_address, data_out_ptr, num_reads, timeout);
}

void 	LIDARLITEV3_write(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout)
{
	uint16_t write_address = (((hLIDARLITEV3->i2c_addr_7)<<1) | 0x00);
	LIDARLITEV3_write_data_buffer[0] = reg_add;
	if((num_writes + 1) > LIDARLITEV3_wdb_num_bytes){ _Error_Handler(__FILE__, __LINE__); }
	for(uint8_t indi = 0; indi < num_writes; indi++)
	{
		LIDARLITEV3_write_data_buffer[indi+1] = *(data_in_ptr + indi);
	}
	HAL_I2C_Master_Transmit(hLIDARLITEV3->hi2c, write_address, &LIDARLITEV3_write_data_buffer[0], num_writes+1, timeout);
}




void 	LIDARLITEV3_assert_i2c_bus(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3)
{
	uint8_t re_init = 0;

	RCC_OscInitTypeDef OscSettings;				// Declare a type to hold the oscillator settings
	RCC_ClkInitTypeDef ClkSettings;				// Declare a type to hold clock settings
	uint32_t FL = __HAL_FLASH_GET_LATENCY();	// Determine the flash latency in order to use the "GetClockConfig" function

	HAL_RCC_GetOscConfig(&OscSettings);			// Read the oscillator settings
	HAL_RCC_GetClockConfig(&ClkSettings, &FL);	// Read the clock settings

	uint32_t SourceFreq = 0;					// Determine the source frequency (skip LSE and LSI because they do not drive the SYSCLK)
	if(OscSettings.HSEState == RCC_HSE_ON){ SourceFreq = HSE_VALUE; }
	else if(OscSettings.HSIState == RCC_HSI_ON){ SourceFreq = HSI_VALUE; }
	else{ _Error_Handler(__FILE__, __LINE__); }


}
