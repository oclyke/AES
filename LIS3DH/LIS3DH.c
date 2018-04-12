/* 
LIS3DH.c

Header: LIS3DH.h

Written by Owen Lyke, April 2018
Updated April 2018

ToDo: 

*/

#include "stm32f4xx_hal.h"
#include "LIS3DH.h"

// Macros




// Functions
// ---------
void LIS3DH_init(LIS3DH_HandleTypeDef *hLIS3DH)
{
	LIS3DH_assert_spi_bus(hLIS3DH); 	// Ensure that the SPI bus is correctly configured for the device

	// CTRLx Registers
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG0, &(hLIS3DH->Init.CTRL1_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL0_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG1, &(hLIS3DH->Init.CTRL1_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL1_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG2, &(hLIS3DH->Init.CTRL2_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL2_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG3, &(hLIS3DH->Init.CTRL3_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL3_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG4, &(hLIS3DH->Init.CTRL4_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL4_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG5, &(hLIS3DH->Init.CTRL5_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL5_REG from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CTRL_REG6, &(hLIS3DH->Init.CTRL6_VAL), 1, hLIS3DH->Timeout); 			// Set CTRL6_REG from user values

	// Other Registers
	LIS3DH_write(hLIS3DH, LIS3DH_REG_FIFO_CTRL_REG, &(hLIS3DH->Init.FIFO_CTRL_VAL), 1, hLIS3DH->Timeout); 	// Set FIFO_CTRL from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_INT1_CFG, 		&(hLIS3DH->Init.INT1_CFG_VAL), 	1, hLIS3DH->Timeout); 			// Set FIFO_CTRL from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_INT2_CFG, 		&(hLIS3DH->Init.INT2_CFG_VAL), 	1, hLIS3DH->Timeout); 			// Set FIFO_CTRL from user values
	LIS3DH_write(hLIS3DH, LIS3DH_REG_CLICK_CFG, 	&(hLIS3DH->Init.CLICK_CFG_VAL), 1, hLIS3DH->Timeout); 		// Set FIFO_CTRL from user values
}


void LIS3DH_read(LIS3DH_HandleTypeDef *hLIS3DH, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x3F) | 0xC0);																												// Specify a multi-byte read beginning with the proper register address
	HAL_GPIO_WritePin(hLIS3DH->CS_GPIO, hLIS3DH->CS_GPIO_Pin, 0); 																							// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hLIS3DH->hspi, &setup, 1, timeout);
	HAL_SPI_Receive(hLIS3DH->hspi, data_out_ptr, num_reads, timeout);
	HAL_GPIO_WritePin(hLIS3DH->CS_GPIO, hLIS3DH->CS_GPIO_Pin, 1); 																							// Set the CS line high to end transmission
}

void LIS3DH_write(LIS3DH_HandleTypeDef *hLIS3DH, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout)
{
	// This function uses blocking SPI because the transmissions will be relatively short, and it simplifies interrupt handling
	uint8_t setup = ((reg_add & 0x3F) | 0x40);																												// Specify a multi-byte write beginning with the proper register address
	HAL_GPIO_WritePin(hLIS3DH->CS_GPIO, hLIS3DH->CS_GPIO_Pin, 0); 																							// Set the CS line low to begin transmission
	HAL_SPI_Transmit(hLIS3DH->hspi, &setup, 1, timeout);
	HAL_SPI_Transmit(hLIS3DH->hspi, data_in_ptr, num_writes, timeout);
	HAL_GPIO_WritePin(hLIS3DH->CS_GPIO, hLIS3DH->CS_GPIO_Pin, 1); 																							// Set the CS line high to end transmission
}


void	LIS3DH_update_accels(LIS3DH_HandleTypeDef *hLIS3DH)
{
	const uint8_t num_reads = 6;
	uint8_t vals[num_reads];

	LIS3DH_assert_spi_bus(hLIS3DH); 	// Ensure that the SPI bus is correctly configured for the device

	LIS3DH_read(hLIS3DH, LIS3DH_REG_OUT_X_L, &vals[0], num_reads, hLIS3DH->Timeout);

	hLIS3DH->X = ((vals[1] << 8) | vals[0]);
	hLIS3DH->Y = ((vals[3] << 8) | vals[2]);
	hLIS3DH->Z = ((vals[5] << 8) | vals[4]);
}

void	LIS3DH_update_temp(LIS3DH_HandleTypeDef *hLIS3DH)
{

}

float	LIS3DH_get_accels_rj(LIS3DH_HandleTypeDef *hLIS3DH, uint16_t * pdata)
{
	// This function is intended to simply give the user a more intuitive Right Justified output, respecting the different power mode settings
	// pdata should point to an array of three uint16_t types, used to represent {X,Y,Z} respectively
	// the returned value of this function can be used to multiply the RJ values to arrive at the value in G's

	uint8_t CR1 = LIS3DH_CTRL_REG1_DEFAULT;												// Prepare to read the values in some registers in order to determine the resolution + conversion factor
	uint8_t CR4 = LIS3DH_CTRL_REG4_DEFAULT;												//
	float scale = 0;
	uint8_t shift = 0;

	LIS3DH_assert_spi_bus(hLIS3DH); 													// Do it
	LIS3DH_read(hLIS3DH, LIS3DH_REG_CTRL_REG1, &CR1, 1, hLIS3DH->Timeout);				//
	LIS3DH_read(hLIS3DH, LIS3DH_REG_CTRL_REG4, &CR4, 1, hLIS3DH->Timeout);				//

	LIS3DH_update_accels(hLIS3DH);														// Take data so that you are sure the settings apply at this time

	// Determine the mode
	if((CR1 & 0x08) && (CR4 & 0x08))
	{
		_Error_Handler(__FILE__, __LINE__); 											// Not allowed to be in both LP (low power) and HR (high resolution) modes at the same time.... whats going wrong?
	}
	else if(CR1 & 0x08){ scale = 0.016; shift = 8; }									// Check for LP mode			   (16 mg/bit, 8 bit)
	else if(CR4 & 0x08){ scale = 0.001; shift = 4; }									// Check for HR mode				(1 mg/bit)
	else			   { scale = 0.004; shift = 6; } 									// Otherwise you're in normal mode 	(4 mg/bit)

	if((hLIS3DH->X) & 0x8000){ *(pdata + 0) = ((hLIS3DH->X >> shift) | (0xFFFF << (16-shift))); } else{ *(pdata + 0) = (hLIS3DH->X >> shift); }
	if((hLIS3DH->Y) & 0x8000){ *(pdata + 1) = ((hLIS3DH->Y >> shift) | (0xFFFF << (16-shift))); } else{ *(pdata + 1) = (hLIS3DH->Y >> shift); }
	if((hLIS3DH->Z) & 0x8000){ *(pdata + 2) = ((hLIS3DH->Z >> shift) | (0xFFFF << (16-shift))); } else{ *(pdata + 2) = (hLIS3DH->Z >> shift); }

	switch(CR4 & 0x30)
	{
		case 0x10 : scale *= 2; break;
		case 0x20 : scale *= 4; break;
		case 0x30 : scale *= 8; break;
	}

	return scale;
}

void	LIS3DH_get_gs(LIS3DH_HandleTypeDef *hLIS3DH, double * pdata)
{
	// This function is intended to provide the acceleration value in Gs (1g = 9.81 m/s2)
	// pdata should point to an array of three double types, used to represent {X,Y,Z} respectively

	uint16_t RJ[3];																		// Hold the RJ values
	float scale = LIS3DH_get_accels_rj(hLIS3DH, &RJ[0]);										// Fill out the RJ values and get scale factor

	*(pdata + 0) = (double)(RJ[0] * scale);												// Now actually scale the data
	*(pdata + 1) = (double)(RJ[1] * scale);
	*(pdata + 2) = (double)(RJ[2] * scale);
}

void 	LIS3DH_assert_spi_bus(LIS3DH_HandleTypeDef *hLIS3DH)
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

	if((hLIS3DH->hspi->Instance == SPI1) || (hLIS3DH->hspi->Instance == SPI4) || (hLIS3DH->hspi->Instance == SPI5))
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
	else if((hLIS3DH->hspi->Instance == SPI2) || (hLIS3DH->hspi->Instance == SPI3))
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

	switch(hLIS3DH->hspi->Init.BaudRatePrescaler){
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

	while((SourceFreq > LIS3DH_MAX_SPI_FREQ) && (hLIS3DH->hspi->Init.BaudRatePrescaler < 0x38))
	{
		hLIS3DH->hspi->Init.BaudRatePrescaler += 0x08;	// If you inspect the pattern of these codes you see that adding 0x08 doubles the prescaler, maximum value of 0x38
		SourceFreq /= 2;								// When you double the prescaler you halve the SourceFreq (which at this point means the final clock frequency)
		re_init = 1;									// Specify that new settings must take place
	}

	// Check if the CPOL and CPHA are correct
	if(hLIS3DH->hspi->Init.CLKPolarity != LIS3DH_CPOL)
	{
		hLIS3DH->hspi->Init.CLKPolarity = LIS3DH_CPOL;
		re_init = 1;
	}
	if(hLIS3DH->hspi->Init.CLKPhase != LIS3DH_CPHA)
	{
		hLIS3DH->hspi->Init.CLKPhase = LIS3DH_CPHA;
		re_init = 1;
	}

	if(re_init)
	{
		if (HAL_SPI_Init(hLIS3DH->hspi) != HAL_OK)		// Attempt to re-initialize the new SPI settings
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		HAL_SPI_Transmit(hLIS3DH->hspi, &re_init, 1, hLIS3DH->Timeout);	// Send a transmission to "fix" the SPI lines
	}
}




