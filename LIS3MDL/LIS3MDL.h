/* 
FXAS21002C.h

Written by Owen Lyke April 2018
Updated April 2018
*/


#ifndef LIS3MDL_H
#define LIS3MDL_H

// Includes
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"




// Check for C++
#ifdef __cplusplus
 extern "C" {
#endif

// ==================================================
//                      Defines
// ==================================================

// SPI parameters of LIS3DH device
// -------------------------------
#define LIS3MDL_CPOL SPI_POLARITY_HIGH		// Idle high
#define LIS3MDL_CPHA SPI_PHASE_2EDGE		// Data valid on second edge (aka rising edge for idle-high)
#define LIS3MDL_MAX_SPI_FREQ 10000000 		// Hz
#define LIS3MDL_SPI_DEFAULT_TIMEOUT 100U 	// Same value as in the HAL library. The units are SysTics, but I'm not 100% sure how that translates to seconds. I think 1 systick is how long the ARM systick counter takes to overflow

// CTRL_REG1
// ---------
#define LIS3MDL_CTRL_REG1_DEFAULT 	0x10	// Power up default value
#define LIS3MDL_TEMPen				0x80 	// Enable temperature sensor
#define LIS3MDL_OMXY_LPM			0x00 	// Low power mode for X and Y axes
#define LIS3MDL_OMXY_MPM			0x20	// Medium-performance mode for X and Y axes
#define LIS3MDL_OMXY_HPM			0x40 	// High-perforance mode for X and Y axes
#define LIS3MDL_OMXY_UHPM			0x60	// Ultrahigh-performance mode for X and Y axes
#define LIS3MDL_ODR_0p625			0x00	// 0.625	Hz output data rate
#define LIS3MDL_ODR_1p25			0x04	// 1.25  	Hz output data rate
#define LIS3MDL_ODR_2p5 			0x08 	// 2.5  	Hz output data rate
#define LIS3MDL_ODR_5 				0x0C 	// 5  		Hz output data rate
#define LIS3MDL_ODR_10 				0x10	// 10 		Hz output data rate
#define LIS3MDL_ODR_20 				0x14 	// 20 		Hz output data rate
#define LIS3MDL_ODR_40 				0x18 	// 40 		Hz output data rate
#define LIS3MDL_ODR_80 				0x1C 	// 80 		Hz output data rate
#define LIS3MDL_FAST_ODRen			0x02 	// Enable fast output data rate, used in conjunction with operating mode setting to determine ODR, up to 1kHz
#define LIS3MDL_STen 				0x01 	// Enable self-test mode

// CTRL_REG2
// ---------
#define LIS3MDL_CTRL_REG2_DEFAULT 	0x00	// Power up default value
#define LIS3MDL_FS_4				0x00	// +/- 4 gauss full-scale range
#define LIS3MDL_FS_8				0x20	// +/- 8 gauss full-scale range
#define LIS3MDL_FS_12				0x40	// +/- 12 gauss full-scale range
#define LIS3MDL_FS_16				0x60	// +/- 16 gauss full-scale range
#define LIS3MDL_REBOOT				0x08 	// Reboot memory content
#define LIS3MDL_SOFT_RST			0x04 	// Configuration registers and user register reset function

// CTRL_REG3
// ---------
#define LIS3MDL_CTRL_REG3_DEFAULT 	0x03	// Power up default value
#define LIS3MDL_LPen				0x20 	// Enable low-power mode. ODR is set to 0.625 Hz
#define LIS3MDL_SIM_3				0x04 	// Enable 3-wire SPI mode
#define LIS3MDL_OM_CONT 			0x00 	// Continuous-conversion mode
#define LIS3MDL_OM_SINGLE 			0x01 	// Single-conversion mode. Single-conversion mode has to be used with sampling frequency from 0.625 Hz to 80Hz.
#define LIS3MDL_OM_PWRDWN 			0x02 	// Power-down mode 

// CTRL_REG4
// ---------
#define LIS3MDL_CTRL_REG4_DEFAULT 	0x00	// Power up default value
#define LIS3MDL_OMZ_LPM				0x00 	// Low-power mode for Z axis
#define LIS3MDL_OMZ_MPM				0x04 	// Medium-performance mode for Z axis
#define LIS3MDL_OMZ_HPM				0x08 	// High-perforance mode for Z axis
#define LIS3MDL_OMZ_UHPM			0x0C	// Ultrahigh-performance mode for Z axis
#define LIS3MDL_BLE_1 				0x02 	// Select data MSB at lower address

// CTRL_REG5
// ---------
#define LIS3MDL_CTRL_REG5_DEFAULT 	0x00	// Power up default value
#define LIS3MDL_FAST_READen 		0x80 	// Allow reading of only high part of DATA OUT to increase reading efficiency
#define LIS3MDL_BDUen				0x40 	// Enable output registers not updated until MSb and LSb have been read

// INT_CFG
// -------
#define LIS3MDL_INT_CFG_DEFAULT 	0xE8	// Power up default value
#define LIS3MDL_XIen 				0x80 	// Enable interrupt on X axis
#define LIS3MDL_YIen 				0x80 	// Enable interrupt on Y axis
#define LIS3MDL_ZIen 				0x80 	// Enable interrupt on Z axis
#define LIS3MDL_IEA_1 				0x80 	// Interrupt active high
#define LIS3MDL_LIR_1 				0x80 	// Interrupt request not latched
#define LIS3MDL_Ien 				0x80 	// Interrupt enabled on INT pin

// ==================================================
//                      Typdefs
// ==================================================
// Register Address Enum
// ---------------------
typedef enum {
	// reserved 0x00 - 0x0E
	LIS3MDL_REG_WHO_AM_I = 0x0F,
	// reserved 0x10 - 0x1F
	LIS3MDL_REG_CTRL_REG1 = 0x20,
	LIS3MDL_REG_CTRL_REG2,
	LIS3MDL_REG_CTRL_REG3,
	LIS3MDL_REG_CTRL_REG4,
	LIS3MDL_REG_CTRL_REG5,
	// reserved 0x25 - 0x26
	LIS3MDL_REG_STATUS_REG = 0x27,
	LIS3MDL_REG_OUT_X_L,
	LIS3MDL_REG_OUT_X_H,
	LIS3MDL_REG_OUT_Y_L,
	LIS3MDL_REG_OUT_Y_H,
	LIS3MDL_REG_OUT_Z_L,
	LIS3MDL_REG_OUT_Z_H,
	LIS3MDL_REG_TEMP_OUT_L,
	LIS3MDL_REG_TEMP_OUT_H,
	LIS3MDL_REG_INT_CFG,
	LIS3MDL_REG_INT_SRC,
	LIS3MDL_REG_INT_THS_L,
	LIS3MDL_REG_INT_THS_H
}LIS3MDL_RegisterTypeDef;


// Initialization Data
// -------------------
typedef struct{
	uint8_t 	CTRL_REG1_VAL;		// Value for F_SETUP, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG2_VAL;		// Value for RT_CFG, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG3_VAL;		// Value for RT_THS, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG4_VAL;		// Value for CTRL0_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG5_VAL;		// Value for CTRL1_REG, constructed by ORing appropriate defined values
	uint8_t 	INT_CFG_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
}LIS3MDL_InitTypeDef;

// Handle 
// ------------------
typedef struct{
	LIS3MDL_InitTypeDef  	Init;			// Initialization parameters for the sensor
	uint32_t				Timeout;		// SPI bus timeout

	SPI_HandleTypeDef		*hspi;			// Which SPI bus is the sensor using?
	GPIO_TypeDef			*CS_GPIO;		// On which GPIO port is the CS line?
	uint16_t 				CS_GPIO_Pin;	// Which pin of that port is the CS line?

	uint8_t 				STATUS;			// Latest status from the STATUS register
	uint16_t				X;				// Latest X-axis data represented as the concatentation of the MSB and LSB registers with MSB first
	uint16_t				Y;				// Latest Y-axis data represented as the concatentation of the MSB and LSB registers with MSB first
	uint16_t				Z;				// Latest Z-axis data represented as the concatentation of the MSB and LSB registers with MSB first
	uint16_t				T;				// Latest temperature data represented as concatenation of MSB and LSB registers with MSB first

}LIS3MDL_HandleTypeDef;




// ==================================================
//                     Functions
// ==================================================
void 	LIS3MDL_init(LIS3MDL_HandleTypeDef *hLIS3MDL);																						// Initialization, uses user-defined parameters. User must completely fill out the initialization structure before initializing

void	LIS3MDL_update_vals(LIS3MDL_HandleTypeDef *hLIS3MDL);																			// Most basic acceleration retrieval. Values in hLIS3DH->[X,Y,Z] look just like concatenation of the MSB and LSB registers for each axis respectively.
void	LIS3MDL_get_mts(LIS3MDL_HandleTypeDef *hLIS3MDL, double * pdata);																	// Get acceleration values in g's, filled into a double array of your choosing

void 	LIS3MDL_read(LIS3MDL_HandleTypeDef *hLIS3MDL, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout);		//
void 	LIS3MDL_write(LIS3MDL_HandleTypeDef *hLIS3MDL, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout);
void 	LIS3MDL_assert_spi_bus(LIS3MDL_HandleTypeDef *hLIS3MDL);



#ifdef __cplusplus
}
#endif

#endif /* LIS3MDL_H */




