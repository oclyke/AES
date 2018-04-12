/* 
FXAS21002C.h

Written by Owen Lyke April 2018
Updated April 2018
*/


#ifndef FXAS21002C_H
#define FXAS21002C_H

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
#define FXAS21002C_CPOL SPI_POLARITY_LOW	// Idle low
#define FXAS21002C_CPHA SPI_PHASE_1EDGE		// Data valid on first edge (aka rising edge for idle-low)
#define FXAS21002C_MAX_SPI_FREQ 2000000 	// Hz
#define FXAS21002C_SPI_DEFAULT_TIMEOUT 100U 			// Same value as in the HAL library. The units are SysTics, but I'm not 100% sure how that translates to seconds. I think 1 systick is how long the ARM systick counter takes to overflow

// F_SETUP
// -------
#define FXAS21002C_F_SETUP_DEFAULT 		0x00	// Default power-on value
#define FXAS21002C_F_DISABLE			0x00 	// FIFO disabled
#define FXAS21002C_F_CIRC				0x40 	// FIFO in circular buffer mode
#define FXAS21002C_F_STOP				0x80 	// FIFO in stop mode

// CTRL_REG0
// ---------
#define FXAS21002C_CTRL_REG0_DEFAULT 	0x00	// Default power-on value
#define FXAS21002C_BW_64				0x80	// 64  Hz bandwidth
#define FXAS21002C_BW_128				0x40	// 128 Hz bandwidth
#define FXAS21002C_BW_256				0x00	// 256 Hz bandwidth
#define FXAS21002C_SPIW_3 				0x20 	// SPI 3-wire mode
//#define FXAS21002C_SEL 				 	0x 	// This could be high-pass filter settings if extended
#define FXAS21002C_HPFen 				0x04 	// Enable high-pass filter (user should specify filter settings)
#define FXAS21002C_FS_2000 				0x00 	// 2000 dps full-scale range
#define FXAS21002C_FS_1000 				0x01 	// 1000 dps full-scale range
#define FXAS21002C_FS_500 				0x02 	// 500 dps full-scale range
#define FXAS21002C_FS_250 				0x03 	// 250 dps full-scale range

// RT_CFG
// ------
#define FXAS21002C_RT_CFG_DEFAULT 		0x00	// Default power-on value
#define FXAS21002C_ELen					0x08 	// Enable event latch
#define FXAS21002C_ZTEFen				0x04 	// Enable event flag on Z axis
#define FXAS21002C_YTEFen				0x02 	// Enable event flag on Y axis
#define FXAS21002C_XTEFen				0x01 	// Enable event flag on X axis

// RT_THS
// ------
#define FXAS21002C_RT_THS_DEFAULT 		0x00	// Default power-on value
#define FXAS21002C_DBCNTM_1				0x80 	// Clear counter when angular rate is below the threshold value
#define FXAS21002C_DBCNTM_0				0x00 	// Decrement counter on every ODR cycle that the angular rate is below the threshold value
//#define FXAS21002C_RT_THS 				  // User sets the threshold, lower 7 bits of this register

// CTRL_REG1
// ---------
#define FXAS21002C_CTRL_REG1_DEFAULT 	0x00	// Default power-on value
#define FXAS21002C_RST 					0x80 	// Trigger a reset of the device
#define FXAS21002C_STen					0x40 	// Enable self-test mode
#define FXAS21002C_ODR_800 				0x00 	// 800  Hz output data rate
#define FXAS21002C_ODR_400				0x04 	// 400  Hz output data rate
#define FXAS21002C_ODR_200				0x08 	// 200  Hz output data rate
#define FXAS21002C_ODR_100				0x0C 	// 100  Hz output data rate
#define FXAS21002C_ODR_50				0x10 	// 50   Hz output data rate
#define FXAS21002C_ODR_12p5 			0x18 	// 12.5 Hz output data rate
#define FXAS21002C_MODE_STNDBY			0x00 	// Standby mode
#define FXAS21002C_MODE_RDY				0x01 	// Ready mode
#define FXAS21002C_MODE_ACTV			0x02 	// Active mode

// CTRL_REG2
// ---------
#define FXAS21002C_CTRL_REG2_DEFAULT 	0x00	// Default power-on value
#define FXAS21002C_FIFO_INT1			0x80 	// Route FIFO interrupt to INT1 pin
#define FXAS21002C_FIFO_INTen			0x40 	// Enable FIFO interrupt
#define FXAS21002C_RT_INT1 				0x20 	// Route RT interrupt to INT1 pin
#define FXAS21002C_RT_INTen 			0x10 	// Enable RT interrupt
#define FXAS21002C_DRDY_INT1 			0x08 	// Route DRDY interrupt to INT1 pin
#define FXAS21002C_DRDY_INTen 			0x04 	// Enable DRDY interrupt
#define FXAS21002C_IPOL_HIGH 			0x02 	// Interrupts active high
#define FXAS21002C_INTPIN_OD 			0x01 	// Interrupt pins made open-drain

// CTRL_REG3
// ---------
#define FXAS21002C_CTRL_REG3_DEFAULT 	0x00	// Default power-on value
#define FXAS21002C_WRAPTOONE 			0x08 	// The auto-increment pointer rolls over to address 0x01 (X-axis MSB) in order to facilitate the faster read out of the FIFO data in a single burst read operation (STATUS register only needs to be read once).
#define FXAS21002C_EXTCTRLen 			0x04 	// INT2 pin becomes an input pin that may be used to control the power mode. Note that when EXTCTRLEN is set, the interrupt outputs and related settings for the INT2 pin are ignored.
#define FXAS21002C_FS_DOUBLE			0x01 	// Maximum full-scale range selections are doubled from what is shown in Table 37


// ==================================================
//                      Typdefs
// ==================================================
// Register Address Enum
// ---------------------
typedef enum {
	FXAS21002C_REG_STATUS = 0x00,
	FXAS21002C_REG_OUT_X_MSB,
	FXAS21002C_REG_OUT_X_LSB,
	FXAS21002C_REG_OUT_Y_MSB,
	FXAS21002C_REG_OUT_Y_LSB,
	FXAS21002C_REG_OUT_Z_MSB,
	FXAS21002C_REG_OUT_Z_LSB,
	FXAS21002C_REG_DR_STATUS,
	FXAS21002C_REG_F_STATUS,
	FXAS21002C_REG_F_SETUP,
	FXAS21002C_REG_F_EVENT,
	FXAS21002C_REG_INT_SRC_FLAG,
	FXAS21002C_REG_WHO_AM_I,
	FXAS21002C_REG_CTRL_REG0,
	FXAS21002C_REG_RT_CFG,
	FXAS21002C_REG_RT_SRC,
	FXAS21002C_REG_RT_THS,
	FXAS21002C_REG_RT_COUNT,
	FXAS21002C_REG_TEMP,
	FXAS21002C_REG_CTRL_REG1,
	FXAS21002C_REG_CTRL_REG2,
	FXAS21002C_REG_CTRL_REG3
	// RESERVED 0x16 - 0xFF
}FXAS21002C_RegisterTypeDef;


// Initialization Data
// -------------------
typedef struct{
	uint8_t 	F_SETUP_VAL;		// Value for F_SETUP, constructed by ORing appropriate defined values
	uint8_t 	RT_CFG_VAL;			// Value for RT_CFG, constructed by ORing appropriate defined values
	uint8_t 	RT_THS_VAL;			// Value for RT_THS, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG0_VAL;		// Value for CTRL0_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG1_VAL;		// Value for CTRL1_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG2_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL_REG3_VAL;		// Value for CTRL3_REG, constructed by ORing appropriate defined values
	
}FXAS21002C_InitTypeDef;

// Handle 
// ------------------
typedef struct{
	FXAS21002C_InitTypeDef  Init;			// Initialization parameters for the sensor
	uint32_t 				Timeout;		// SPI bus timeout

	SPI_HandleTypeDef		*hspi;			// Which SPI bus is the sensor using?
	GPIO_TypeDef			*CS_GPIO;		// On which GPIO port is the CS line?
	uint16_t 				CS_GPIO_Pin;	// Which pin of that port is the CS line?

	uint8_t 				STATUS;			// Latest status from the STATUS register
	uint16_t				X;				// The latest data from the X axis presented as a direct concatenation of the X MSB and LSB as [MSB:LSB]
	uint16_t				Y;				// The latest data from the Y axis presented as a direct concatenation of the Y MSB and LSB as [MSB:LSB]
	uint16_t				Z;				// The latest data from the Z axis presented as a direct concatenation of the Z MSB and LSB as [MSB:LSB]
	uint8_t					T;				// 8bit 2's complement temperature from -128 to 127 deg C, only valid in active mode when measuring rotational rates

}FXAS21002C_HandleTypeDef;




// ==================================================
//                     Functions
// ==================================================
void 	FXAS21002C_init(FXAS21002C_HandleTypeDef *hFXAS21002C);																						// Initialization, uses user-defined parameters. User must completely fill out the initialization structure before initializing

void	FXAS21002C_update_vals(FXAS21002C_HandleTypeDef *hFXAS21002C);																			// Most basic acceleration retrieval. Values in hLIS3DH->[X,Y,Z] look just like concatenation of the MSB and LSB registers for each axis respectively.
void	FXAS21002C_update_temp(FXAS21002C_HandleTypeDef *hFXAS21002C);																				// Not used so far
void	FXAS21002C_get_dps(FXAS21002C_HandleTypeDef *hFXAS21002C, double * pdata);																	// Get acceleration values in g's, filled into a double array of your choosing

void 	FXAS21002C_read(FXAS21002C_HandleTypeDef *hFXAS21002C, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout);		//
void 	FXAS21002C_write(FXAS21002C_HandleTypeDef *hFXAS21002C, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout);
void 	FXAS21002C_assert_spi_bus(FXAS21002C_HandleTypeDef *hFXAS21002C);



#ifdef __cplusplus
}
#endif

#endif /* FXAS21002C_H */




