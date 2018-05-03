/* 
FXAS21002C.c

Header: FXAS21002C.h

Written by Owen Lyke, April 2018
Updated April 2018

ToDo: 

*/

// Macros
#include "FXAS21002C.h"

// Functions
// ---------
void FXAS21002C_init(FXAS21002C_HandleTypeDef *hFXAS21002C)
{
	FXAS21002C_assert_spi_bus(hFXAS21002C);																						// Ensure the SPI bus is ready for this sensor

	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_F_SETUP, &(hFXAS21002C->Init.F_SETUP_VAL), 1, hFXAS21002C->Timeout); 				// Set F_SETUP from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_RT_CFG, &(hFXAS21002C->Init.RT_CFG_VAL), 1, hFXAS21002C->Timeout); 					// Set RT_CFG from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_RT_THS, &(hFXAS21002C->Init.RT_THS_VAL), 1, hFXAS21002C->Timeout); 					// Set RT_THS from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_CTRL_REG0, &(hFXAS21002C->Init.CTRL_REG0_VAL), 1, hFXAS21002C->Timeout); 			// Set CTRL_REG0 from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_CTRL_REG1, &(hFXAS21002C->Init.CTRL_REG1_VAL), 1, hFXAS21002C->Timeout); 			// Set CTRL_REG1 from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_CTRL_REG2, &(hFXAS21002C->Init.CTRL_REG2_VAL), 1, hFXAS21002C->Timeout); 			// Set CTRL_REG2 from user values
	FXAS21002C_write(hFXAS21002C, FXAS21002C_REG_CTRL_REG3, &(hFXAS21002C->Init.CTRL_REG3_VAL), 1, hFXAS21002C->Timeout); 			// Set CTRL_REG3 from user values
}

void FXAS21002C_read(FXAS21002C_HandleTypeDef *hFXAS21002C, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x7F) | 0x80);																									// Specify a read beginning with the proper register address
	HAL_GPIO_WritePin(hFXAS21002C->CS_GPIO, hFXAS21002C->CS_GPIO_Pin, 0); 																		// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hFXAS21002C->hspi, &setup, 1, timeout);
	HAL_SPI_Receive(hFXAS21002C->hspi, data_out_ptr, num_reads, timeout);
	HAL_GPIO_WritePin(hFXAS21002C->CS_GPIO, hFXAS21002C->CS_GPIO_Pin, 1); 																		// Set the CS line high to end transmission
}

void FXAS21002C_write(FXAS21002C_HandleTypeDef *hFXAS21002C, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x7F) | 0x00);																									// Specify a write beginning with the proper register address
	HAL_GPIO_WritePin(hFXAS21002C->CS_GPIO, hFXAS21002C->CS_GPIO_Pin, 0); 																		// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hFXAS21002C->hspi, &setup, 1, timeout);
	HAL_SPI_Transmit(hFXAS21002C->hspi, data_in_ptr, num_writes, timeout);
	HAL_GPIO_WritePin(hFXAS21002C->CS_GPIO, hFXAS21002C->CS_GPIO_Pin, 1); 																		// Set the CS line high to end transmission
}


void	FXAS21002C_update_vals(FXAS21002C_HandleTypeDef *hFXAS21002C)
{
	const uint8_t num_reads = 7;
	uint8_t vals[num_reads];

	FXAS21002C_assert_spi_bus(hFXAS21002C); 	// Ensure that the SPI bus is correctly configured for the device

	FXAS21002C_read(hFXAS21002C, FXAS21002C_REG_STATUS, &vals[0], num_reads, hFXAS21002C->Timeout);

	hFXAS21002C->STATUS = vals[0];
	hFXAS21002C->X = ((vals[1] << 8) | vals[2]);
	hFXAS21002C->Y = ((vals[3] << 8) | vals[4]);
	hFXAS21002C->Z = ((vals[5] << 8) | vals[6]);
}

void	FXAS21002C_update_temp(FXAS21002C_HandleTypeDef *hFXAS21002C)
{
	FXAS21002C_read(hFXAS21002C, FXAS21002C_REG_TEMP, &(hFXAS21002C->T), 1, hFXAS21002C->Timeout);
}

void	FXAS21002C_get_dps(FXAS21002C_HandleTypeDef *hFXAS21002C, double * pdata)
{
	// This function is intended to provide the acceleration value in Gs (1g = 9.81 m/s2)
	// pdata should point to an array of three double types, used to represent {X,Y,Z} respectively
	uint8_t CR0 = FXAS21002C_CTRL_REG0_DEFAULT;
	uint8_t CR3 = FXAS21002C_CTRL_REG3_DEFAULT;

	FXAS21002C_assert_spi_bus(hFXAS21002C);
	FXAS21002C_read(hFXAS21002C, FXAS21002C_REG_CTRL_REG0, &CR0, 1, hFXAS21002C->Timeout);
	FXAS21002C_read(hFXAS21002C, FXAS21002C_REG_CTRL_REG3, &CR3, 1, hFXAS21002C->Timeout);

	FXAS21002C_update_vals(hFXAS21002C);

	double scale;
	switch( CR0 & 0x03 ){
		case 0x00 : scale = 62.5; break;
		case 0x01 : scale = 31.25; break;
		case 0x02 : scale = 15.625; break;
		case 0x03 : scale = 7.8125; break;
	}
	if( CR3 & 0x01 )
	{
		scale *= 2;
	}

	*(pdata + 0) = (double)(hFXAS21002C->X * scale);												// Now actually scale the data
	*(pdata + 1) = (double)(hFXAS21002C->Y * scale);
	*(pdata + 2) = (double)(hFXAS21002C->Z * scale);
}

void 	FXAS21002C_assert_spi_bus(FXAS21002C_HandleTypeDef *hFXAS21002C)
{
	// Use the HAL RCC library to get the status of the system clocks in order to adjust clock speed if necessary

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

	// Now use the source frequency and the oscillator/clock/spi settings to determine the SCLK frequency
	if(OscSettings.PLL.PLLState == RCC_PLL_ON){
		SourceFreq *= (OscSettings.PLL.PLLN / (OscSettings.PLL.PLLM * OscSettings.PLL.PLLP));
	}

	switch(ClkSettings.AHBCLKDivider){
		case RCC_SYSCLK_DIV1 : SourceFreq /= 1; break;
		case RCC_SYSCLK_DIV2 : SourceFreq /= 2; break;
		case RCC_SYSCLK_DIV4 : SourceFreq /= 4; break;
		case RCC_SYSCLK_DIV8 : SourceFreq /= 8; break;
		case RCC_SYSCLK_DIV16 : SourceFreq /= 16; break;
		case RCC_SYSCLK_DIV64 : SourceFreq /= 64; break;
		case RCC_SYSCLK_DIV128 : SourceFreq /= 128; break;
		case RCC_SYSCLK_DIV256 : SourceFreq /= 256; break;
		case RCC_SYSCLK_DIV512 : SourceFreq /= 512; break;
		default : _Error_Handler(__FILE__, __LINE__);
	}

	if((hFXAS21002C->hspi->Instance == SPI1) || (hFXAS21002C->hspi->Instance == SPI4) || (hFXAS21002C->hspi->Instance == SPI5))
	{
		// These ports use the APB2 clock
		switch(ClkSettings.APB2CLKDivider){
			case RCC_HCLK_DIV1 : SourceFreq /= 1; break;
			case RCC_HCLK_DIV2 : SourceFreq /= 2; break;
			case RCC_HCLK_DIV4 : SourceFreq /= 4; break;
			case RCC_HCLK_DIV8 : SourceFreq /= 8; break;
			case RCC_HCLK_DIV16 : SourceFreq /= 16; break;
			default : _Error_Handler(__FILE__, __LINE__);
		}
	}
	else if((hFXAS21002C->hspi->Instance == SPI2) || (hFXAS21002C->hspi->Instance == SPI3))
	{
		// These ports use the APB1 clock
		switch(ClkSettings.APB1CLKDivider){
			case RCC_HCLK_DIV1 : SourceFreq /= 1; break;
			case RCC_HCLK_DIV2 : SourceFreq /= 2; break;
			case RCC_HCLK_DIV4 : SourceFreq /= 4; break;
			case RCC_HCLK_DIV8 : SourceFreq /= 8; break;
			case RCC_HCLK_DIV16 : SourceFreq /= 16; break;
			default : _Error_Handler(__FILE__, __LINE__);
		}
	}
	else{ _Error_Handler(__FILE__, __LINE__); } // Oops, looks like you don't have a SPI port?

	switch(hFXAS21002C->hspi->Init.BaudRatePrescaler){
		case SPI_BAUDRATEPRESCALER_2 : SourceFreq /= 2; break;
		case SPI_BAUDRATEPRESCALER_4 : SourceFreq /= 4; break;
		case SPI_BAUDRATEPRESCALER_8 : SourceFreq /= 8; break;
		case SPI_BAUDRATEPRESCALER_16 : SourceFreq /= 16; break;
		case SPI_BAUDRATEPRESCALER_32: SourceFreq /= 32; break;
		case SPI_BAUDRATEPRESCALER_64 : SourceFreq /= 64; break;
		case SPI_BAUDRATEPRESCALER_128 : SourceFreq /= 128; break;
		case SPI_BAUDRATEPRESCALER_256 : SourceFreq /= 256; break;
		default : _Error_Handler(__FILE__, __LINE__);
	} // SourceFreq finally contains the SPI clock frequency

	while((SourceFreq > FXAS21002C_MAX_SPI_FREQ) && (hFXAS21002C->hspi->Init.BaudRatePrescaler < 0x38))
	{
		hFXAS21002C->hspi->Init.BaudRatePrescaler += 0x08;	// If you inspect the pattern of these codes you see that adding 0x08 doubles the prescaler, maximum value of 0x38
		SourceFreq /= 2;								// When you double the prescaler you halve the SourceFreq (which at this point means the final clock frequency)
		re_init = 1;									// Specify that new settings must take place
	}

	// Check if the CPOL and CPHA are correct
	if(hFXAS21002C->hspi->Init.CLKPolarity != FXAS21002C_CPOL)
	{
		hFXAS21002C->hspi->Init.CLKPolarity = FXAS21002C_CPOL;
		re_init = 1;
	}
	if(hFXAS21002C->hspi->Init.CLKPhase != FXAS21002C_CPHA)
	{
		hFXAS21002C->hspi->Init.CLKPhase = FXAS21002C_CPHA;
		re_init = 1;
	}

	if(re_init)
	{
		if (HAL_SPI_Init(hFXAS21002C->hspi) != HAL_OK)		// Attempt to re-initialize the new SPI settings
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		HAL_SPI_Transmit(hFXAS21002C->hspi, &re_init, 1, hFXAS21002C->Timeout);
	}
}
