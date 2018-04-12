/* 
LIS3MDL.c

Header: LIS3MDL.h

Written by Owen Lyke, April 2018
Updated April 2018

ToDo:

*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "LIS3MDL.h"


// Macros


// Functions
// ---------
void LIS3MDL_init(LIS3MDL_HandleTypeDef *hLIS3MDL)
{
	LIS3MDL_assert_spi_bus(hLIS3MDL);																						// Ensure the SPI bus is ready for this sensor

	// CTRLx Registers
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_CTRL_REG1, 	&(hLIS3MDL->Init.CTRL_REG1_VAL), 	1, hLIS3MDL->Timeout); 				// Set CTRL1 from user values
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_CTRL_REG2, 	&(hLIS3MDL->Init.CTRL_REG2_VAL), 	1, hLIS3MDL->Timeout); 				// Set CTRL2 from user values
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_CTRL_REG3, 	&(hLIS3MDL->Init.CTRL_REG3_VAL), 	1, hLIS3MDL->Timeout); 				// Set CTRL3 from user values
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_CTRL_REG4, 	&(hLIS3MDL->Init.CTRL_REG4_VAL), 	1, hLIS3MDL->Timeout); 				// Set CTRL4 from user values
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_CTRL_REG5, 	&(hLIS3MDL->Init.CTRL_REG5_VAL), 	1, hLIS3MDL->Timeout); 				// Set CTRL5 from user values
	LIS3MDL_write(hLIS3MDL, LIS3MDL_REG_INT_CFG, 	&(hLIS3MDL->Init.INT_CFG_VAL), 		1, hLIS3MDL->Timeout); 				// Set INT_CFG from user values
}

void LIS3MDL_read(LIS3MDL_HandleTypeDef *hLIS3MDL, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x3F) | 0xC0);																									// Specify a read beginning with the proper register address
	HAL_GPIO_WritePin(hLIS3MDL->CS_GPIO, hLIS3MDL->CS_GPIO_Pin, 0); 																		// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hLIS3MDL->hspi, &setup, 1, timeout);
	HAL_SPI_Receive(hLIS3MDL->hspi, data_out_ptr, num_reads, timeout);
	HAL_GPIO_WritePin(hLIS3MDL->CS_GPIO, hLIS3MDL->CS_GPIO_Pin, 1); 																		// Set the CS line high to end transmission
}

void LIS3MDL_write(LIS3MDL_HandleTypeDef *hLIS3MDL, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x3F) | 0x40);																									// Specify a write beginning with the proper register address
	HAL_GPIO_WritePin(hLIS3MDL->CS_GPIO, hLIS3MDL->CS_GPIO_Pin, 0); 																		// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hLIS3MDL->hspi, &setup, 1, timeout);
	HAL_SPI_Transmit(hLIS3MDL->hspi, data_in_ptr, num_writes, timeout);
	HAL_GPIO_WritePin(hLIS3MDL->CS_GPIO, hLIS3MDL->CS_GPIO_Pin, 1); 																		// Set the CS line high to end transmission
}


void	LIS3MDL_update_vals(LIS3MDL_HandleTypeDef *hLIS3MDL)
{
	const uint8_t num_reads = 9;
	uint8_t vals[num_reads];

	LIS3MDL_assert_spi_bus(hLIS3MDL); 	// Ensure that the SPI bus is correctly configured for the device

	LIS3MDL_read(hLIS3MDL, LIS3MDL_REG_STATUS_REG, &vals[0], num_reads, hLIS3MDL->Timeout);

	hLIS3MDL->STATUS = vals[0];
	hLIS3MDL->X = ((vals[2] << 8) | vals[1]);
	hLIS3MDL->Y = ((vals[4] << 8) | vals[3]);
	hLIS3MDL->Z = ((vals[6] << 8) | vals[5]);
	hLIS3MDL->T = ((vals[8] << 8) | vals[7]);
}

void	LIS3MDL_get_guass(LIS3MDL_HandleTypeDef *hLIS3MDL, double * pdata)
{
	// This function is intended to provide the acceleration value in Gs (1g = 9.81 m/s2)
	// pdata should point to an array of three double types, used to represent {X,Y,Z} respectively
	uint8_t CR2 = LIS3MDL_CTRL_REG2_DEFAULT;
	uint8_t CR4 = LIS3MDL_CTRL_REG4_DEFAULT;

	LIS3MDL_assert_spi_bus(hLIS3MDL);
	LIS3MDL_read(hLIS3MDL, LIS3MDL_REG_CTRL_REG2, &CR2, 1, hLIS3MDL->Timeout);
	LIS3MDL_read(hLIS3MDL, LIS3MDL_REG_CTRL_REG4, &CR4, 1, hLIS3MDL->Timeout);

	LIS3MDL_update_vals(hLIS3MDL);

	double scale = 0.000122070312; // Scale for +/-4guass assuming full 16-bit
	switch( (CR2 & 0x60) >> 5 ){
		case 0x00 : scale *= 1; break;					// Unity scale for +/-4guass
		case 0x01 : scale *= 2; break;					// Double scale for +/-8guass
		case 0x02 : scale *= 3; break;					// Triple scale for +/-12guass
		case 0x03 : scale *= 4; break;					// Quadruple scale for +/-16guass
	}
	if( CR4 & 0x02 )
	{
		hLIS3MDL->X = (((hLIS3MDL->X & 0x00FF) << 8) | ((hLIS3MDL->X & 0xFF00) >> 8) );			// Flip the endianness of the variables because of this bit being set
		hLIS3MDL->Y = (((hLIS3MDL->Y & 0x00FF) << 8) | ((hLIS3MDL->Y & 0xFF00) >> 8) );			// Flip the endianness of the variables because of this bit being set
		hLIS3MDL->Z = (((hLIS3MDL->Z & 0x00FF) << 8) | ((hLIS3MDL->Z & 0xFF00) >> 8) );			// Flip the endianness of the variables because of this bit being set
	}

	*(pdata + 0) = (double)(hLIS3MDL->X * scale);												// Now actually scale the data
	*(pdata + 1) = (double)(hLIS3MDL->Y * scale);
	*(pdata + 2) = (double)(hLIS3MDL->Z * scale);
}

void 	LIS3MDL_assert_spi_bus(LIS3MDL_HandleTypeDef *hLIS3MDL)
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

	if((hLIS3MDL->hspi->Instance == SPI1) || (hLIS3MDL->hspi->Instance == SPI4) || (hLIS3MDL->hspi->Instance == SPI5))
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
	else if((hLIS3MDL->hspi->Instance == SPI2) || (hLIS3MDL->hspi->Instance == SPI3))
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

	switch(hLIS3MDL->hspi->Init.BaudRatePrescaler){
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

	while((SourceFreq > LIS3MDL_MAX_SPI_FREQ) && (hLIS3MDL->hspi->Init.BaudRatePrescaler < 0x38))
	{
		hLIS3MDL->hspi->Init.BaudRatePrescaler += 0x08;	// If you inspect the pattern of these codes you see that adding 0x08 doubles the prescaler, maximum value of 0x38
		SourceFreq /= 2;								// When you double the prescaler you halve the SourceFreq (which at this point means the final clock frequency)
		re_init = 1;									// Specify that new settings must take place
	}

	// Check if the CPOL and CPHA are correct
	if(hLIS3MDL->hspi->Init.CLKPolarity != LIS3MDL_CPOL)
	{
		hLIS3MDL->hspi->Init.CLKPolarity = LIS3MDL_CPOL;
		re_init = 1;
	}
	if(hLIS3MDL->hspi->Init.CLKPhase != LIS3MDL_CPHA)
	{
		hLIS3MDL->hspi->Init.CLKPhase = LIS3MDL_CPHA;
		re_init = 1;
	}

	if(re_init)
	{
		if (HAL_SPI_Init(hLIS3MDL->hspi) != HAL_OK)		// Attempt to re-initialize the new SPI settings
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		HAL_SPI_Transmit(hLIS3MDL->hspi, &re_init, 1, hLIS3MDL->Timeout);	// Send a transmission to "fix" the SPI lines
	}

}

