/* 
LIS3DH.h

Written by Owen Lyke April 2018
Updated April 2018
*/


#ifndef LIS3DH_H
#define LIS3DH_H

// Includes




// Check for C++
#ifdef __cplusplus
 extern "C" {
#endif

// ==================================================
//                      Defines
// ==================================================

// SPI parameters of LIS3DH device
// -------------------------------
#define LIS3DH_CPOL SPI_POLARITY_HIGH	// Idle high
#define LIS3DH_CPHA SPI_PHASE_2EDGE		// Data valid on second edge (aka rising edge for idle-high)
#define LIS3DH_MAX_SPI_FREQ 10000000 	// Hz
#define LIS3DH_SPI_DEFAULT_TIMEOUT 100U 		// Same value as in the HAL library. The units are SysTics, but I'm not 100% sure how that translates to seconds. I think 1 systick is how long the ARM systick counter takes to overflow

// CTRL_REG0
// ---------
#define LIS3DH_CTRL_REG0_DEFAULT	0x10	// Default boot-up value
#define LIS3DH_SDO_PU_DISC			0x90 	// Disconnect the internal pull-up

// TEMP_CFG_REG
// ------------
#define LIS3DH_TEMP_CFG_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_TEMPen				0x40 	// Enable the temperature sensor
#define LIS3DH_ADCen				0x80	// Enable the ADC

// CTRL_REG1
// ---------
#define LIS3DH_CTRL_REG1_DEFAULT	0x07	// Default boot-up value
#define LIS3DH_ODR_PWRDWN			0x00 	// Power-down mode
#define LIS3DH_ODR_1				0x10 	// 1   Hz in all modes
#define LIS3DH_ODR_10				0x20 	// 10  Hz in all modes
#define LIS3DH_ODR_25				0x30 	// 25  Hz in all modes
#define LIS3DH_ODR_50				0x40 	// 50  Hz in all modes
#define LIS3DH_ODR_100				0x50 	// 100 Hz in all modes
#define LIS3DH_ODR_200				0x60 	// 200 Hz in all modes
#define LIS3DH_ODR_400				0x70 	// 400 Hz in all modes
#define LIS3DH_ODR_1k6_LP			0x80	// 1.60  kHz in low-power mode
#define LIS3DH_ODR_1k344_HR			0x90	// 1.344 kHz in high-resolution mode
#define LIS3DH_ODR_1k344_NOR		0x90 	// 1.344 kHz in high-resolution mode
#define LIS3DH_ODR_5k376_LP			0x90	// 5.376 kHz in low-power mode
#define LIS3DH_LPen					0x08 	// Enables low-power mode
#define LIS3DH_Zen					0x04 	// Enables Z-axis						(hint use !LIS3DH_Zen in initialization to disable)
#define LIS3DH_Yen					0x02 	// Enables Y-axis
#define LIS3DH_Xen					0x01 	// Enables X-axis

// CTRL_REG2
// ---------
#define LIS3DH_CTRL_REG2_DEFAULT	0x00	// Default boot-up value
#define LIS3DH_HPM_NORM				0x00 	// Normal mode (reset by reading REFERENCE (26h))
#define LIS3DH_HPM_REF				0x04 	// Reference signal for filtering
#define LIS3DH_HPM_AUTO				0xC0	// Autoreset on interrupt event
//#define LIS3DH_HPCF_??		0x??
//#define LIS3DH_HPCF_??		0x??
//#define LIS3DH_HPCF_??		0x??
//#define LIS3DH_HPCF_??		0x??
#define LIS3DH_FDen					0x08 	// Internal filter enabled
#define LIS3DH_HPCLICKen			0x04 	// High-pass filter enabled for CLICK function
#define LIS3DH_HP_IA2en				0x02 	// High-pass filter enabled for AOI function on interrupt 2
#define LIS3DH_HP_IA1en				0x01 	// High-pass filter enabled for AOI function on interrupt 1

// CTRL_REG3
// ---------
#define LIS3DH_CTRL_REG3_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_I1_CLICKen 			0x80 	// Enable click interrupt on INT1
#define LIS3DH_I1_IA1en				0x40 	// Enable IA1 interrupt on INT1
#define LIS3DH_I1_IA2en				0x20 	// Enable IA2 interrupt on INT1
#define LIS3DH_I1_ZYXDAen			0x10 	// Enable ZYXDA interrupt on INT1
#define LIS3DH_I1_321DAen			0x08 	// Enable 321DA interrupt on INT1
#define LIS3DH_I1_WTMen				0x04 	// Enable FIFO watermark interrupt on INT1
#define LIS3DH_I1_OVERRUNen			0x02 	// Enable FIFO overrun interrupt on INT1

// CTRL_REG4
// ---------
#define LIS3DH_CTRL_REG4_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_BDUen				0x80 	// Enable block data update so that data not updated until both LSB and MSB readings are taken
#define LIS3DH_BLE_LE				0x00 	// Data LSB is in lower address register (high-resolution mode only)
#define LIS3DH_BLE_BE				0x60 	// Data LSB is in higher address register (high-resolution mode only)
#define LIS3DH_FS_2g 				0x00 	// +/- 2g full-scale resolution
#define LIS3DH_FS_4g				0x10 	// +/- 4g full-scale resolution
#define LIS3DH_FS_8g				0x20 	// +/- 8g full-scale resolution
#define LIS3DH_FS_16g				0x30 	// +/- 16g full-scale resolution
#define LIS3DH_HRen					0x08 	// Enable high-resolution mode
#define LIS3DH_ST_OFF 				0x00 	// Self-test disabled
#define LIS3DH_ST_0					0x02 	// Self-test mode 0
#define LIS3DH_ST_1					0x04 	// Self-test mode 1
#define LIS3DH_ST_2					0x06 	// Self-test "--" (whatever that means)
#define LIS3DH_SIM_4				0x00 	// 4-wire SPI interface
#define LIS3DH_SIM_3				0x01 	// 3-wire SPI interface

// CTRL_REG5
// ---------
//#define LIS3DH_BOOT
#define LIS3DH_CTRL_REG5_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_FIFOen				0x60 	// Enable the FIFO buffer
#define LIS3DH_LIR_INT1en 			0x08 	// Latch interrupt request on INT1_SRC register, with INT1_SRC (31h) register cleared by reading INT1_SRC (31h) itself. Default value: 0. (0: interrupt request not latched; 1: interrupt request latched)
#define LIS3DH_D4D_INT1en 			0x04 	// 4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
#define LIS3DH_LIR_INT2en 			0x02 	// Latch interrupt request on INT2_SRC (35h) register, with INT2_SRC (35h) register cleared by reading INT2_SRC (35h) itself.
#define LIS3DH_D4D_INT2en 			0x01 	// 4D enable: 4D detection is enabled on INT2 pin when 6D bit on INT2_CFG (34h) is set to 1.

// CTRL_REG6
// ---------
#define LIS3DH_CTRL_REG6_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_I2_CLICKen 			0x80 	// Click interrupt on INT2 pin
#define LIS3DH_I2_IA1en				0x40 	// Enable interrupt 1 function on INT2 pin
#define LIS3DH_I2_IA2en 			0x20 	// Enable interrupt 2 function on INT2 pin
#define LIS3DH_I2_BOOTen	 		0x10 	// Enable boot on INT2 pin
#define LIS3DH_I2_ACTen				0x08 	// Enable activity interrupt on INT2 pin
#define LIS3DH_INT_POL_ACTH			0x00 	// INT1 and INT2 pin polarity active high
#define LIS3DH_INT_POL_ACTL			0x02 	// INT1 and INT2 pin polarity active low

// FIFO_CTRL_REG
// -------------
#define LIS3DH_FIFO_CTRL_REG_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_FM_BYP 			0x00 	// Bypass mode
#define LIS3DH_FM_FIFO 			0x40 	// FIFO mode
#define LIS3DH_FM_STREAM	  	0x80 	// Stream mode
#define LIS3DH_FM_STRFIFO		0xC0 	// Stream-to-FIFO

// INTx_CFG
// ------------
#define LIS3DH_INTx_CFG_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_AOI_1 				0x80 	// And/Or combination of Interrupt events. Default value: 0 Refer to Table 55: Interrupt mode
#define LIS3DH_AOI_0 				0x00 	// And/Or combination of Interrupt events. Default value: 0 Refer to Table 55: Interrupt mode
#define LIS3DH_6D_1 				0x40 	// 6 direction detection function enabled. Default value: 0Refer to Table 55: Interrupt mode
#define LIS3DH_6D_0 				0x00 	// 6 direction detection function enabled. Default value: 0Refer to Table 55: Interrupt mode
#define LIS3DH_ZHIen				0x20 	// Enable interrupt generation on Z high event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)
#define LIS3DH_ZLIen				0x10 	// Enable interrupt generation on Z low event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)
#define LIS3DH_YHIen				0x08 	// Enable interrupt generation on Y high event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)
#define LIS3DH_YLIen				0x04 	// Enable interrupt generation on Y low event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)
#define LIS3DH_XHIen				0x02 	// Enable interrupt generation on X high event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)
#define LIS3DH_XLIen				0x01 	// Enable interrupt generation on X low event or on Direction recognition. Default value: 0 (0: disable interrupt request;1: enable interrupt request)

// CLICK_CFG
// ---------
#define LIS3DH_CLICK_CFG_DEFAULT 	0x00	// Default boot-up value
#define LIS3DH_ZDen 				0x20 	// Enable interrupt double click on Z-axis
#define LIS3DH_ZSen 				0x10 	// Enable interrupt single click on Z-axis
#define LIS3DH_YDen 				0x08 	// Enable interrupt double click on Y-axis
#define LIS3DH_YSen 				0x04 	// Enable interrupt single click on Y-axis
#define LIS3DH_XDen 				0x02 	// Enable interrupt double click on X-axis
#define LIS3DH_XSen 				0x01 	// Enable interrupt single click on X-axis




// Typdefs

// Register Address Enum
// ---------------------
typedef enum {
	LIS3DH_REG_STATUS_REG_AUX 	= 0x07,
	LIS3DH_REG_OUT_ADC1_L,
	LIS3DH_REG_OUT_ADC1_H,
	LIS3DH_REG_OUT_ADC2_L,
	LIS3DH_REG_OUT_ADC2_H,
	LIS3DH_REG_OUT_ADC3_L,
	LIS3DH_REG_OUT_ADC3_H,
	// Reserved 0x0E
	LIS3DH_REG_WHO_AM_I 		= 0x0F,
	// Reserved 0x10-0x1D
	LIS3DH_REG_CTRL_REG0		= 0x1E,
	LIS3DH_REG_TEMP_CFG_REG,
	LIS3DH_REG_CTRL_REG1,
	LIS3DH_REG_CTRL_REG2,
	LIS3DH_REG_CTRL_REG3,
	LIS3DH_REG_CTRL_REG4,
	LIS3DH_REG_CTRL_REG5,
	LIS3DH_REG_CTRL_REG6,
	LIS3DH_REG_REFERENCE,
	LIS3DH_REG_STATUS_REG,
	LIS3DH_REG_OUT_X_L,
	LIS3DH_REG_OUT_X_H,
	LIS3DH_REG_OUT_Y_L,
	LIS3DH_REG_OUT_Y_H,
	LIS3DH_REG_OUT_Z_L,
	LIS3DH_REG_OUT_Z_H,
	LIS3DH_REG_FIFO_CTRL_REG,
	LIS3DH_REG_FIFO_SRC_REG,
	LIS3DH_REG_INT1_CFG,
	LIS3DH_REG_INT1_SRC,
	LIS3DH_REG_INT1_THS,
	LIS3DH_REG_INT1_DURATION,
	LIS3DH_REG_INT2_CFG,
	LIS3DH_REG_INT2_SRC,
	LIS3DH_REG_INT2_THS,
	LIS3DH_REG_INT2_DURATION,
	LIS3DH_REG_CLICK_CFG,
	LIS3DH_REG_CLICK_SRC,
	LIS3DH_REG_CLICK_THS,
	LIS3DH_REG_TIME_LIMIT,
	LIS3DH_REG_TIME_LATENCY,
	LIS3DH_REG_TIME_WINDOW,
	LIS3DH_REG_ACT_THS,
	LIS3DH_REG_ACT_DUR
} LIS3DH_RegisterTypeDef;


// Initialization Data
// -------------------
typedef struct{
	uint8_t		CTRL0_VAL;		// Value for CTRL0_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL1_VAL;		// Value for CTRL1_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL2_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL3_VAL;		// Value for CTRL3_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL4_VAL;		// Value for CTRL4_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL5_VAL;		// Value for CTRL5_REG, constructed by ORing appropriate defined values
	uint8_t 	CTRL6_VAL;		// Value for CTRL6_REG, constructed by ORing appropriate defined values
	uint8_t		TEMP_CFG_VAL;	// Value for TEMP_CFG_REG, constructed by ORing appropriate defined values
	uint8_t		FIFO_CTRL_VAL;	// Value for CTRL6_REG, constructed by ORing appropriate defined values
	uint8_t		INT1_CFG_VAL;	// Value for INT1_CFG, constructed by ORing appropriate defined values
	uint8_t		INT2_CFG_VAL;	// Value for INT1_CFG, constructed by ORing appropriate defined values
	uint8_t		CLICK_CFG_VAL;	// Value for CLICK_CFG, constructed by ORing appropriate defined values
}LIS3DH_InitTypeDef;

// Handle 
// ------------------
typedef struct{
	LIS3DH_InitTypeDef		Init;			// Initialization settings for the sensor
	uint32_t 				Timeout;		// How long to wait for SPI

	SPI_HandleTypeDef		*hspi;			// Which SPI bus is the sensor using?
	GPIO_TypeDef			*CS_GPIO;		// On which GPIO port is the CS line?
	uint16_t 				CS_GPIO_Pin;	// Which pin of that port is the CS line?

	uint16_t 				X;				// The most recent X-axis reading formatted in twos complement and left justified
	uint16_t 				Y;				// The most recent Y-axis reading formatted in twos complement and left justified
	uint16_t 				Z;				// The most recent Z-axis reading formatted in twos complement and left justified
	uint16_t 				T;				// The most recent temperature reading formatted in twos complement and left justified

}LIS3DH_HandleTypeDef;





// ==================================================
//                     Functions
// ==================================================
void 	LIS3DH_init(LIS3DH_HandleTypeDef *hLIS3DH);																						// Initialization, uses user-defined parameters. User must completely fill out the initialization structure before initializing

void	LIS3DH_update_accels(LIS3DH_HandleTypeDef *hLIS3DH);																			// Most basic acceleration retrieval. Values in hLIS3DH->[X,Y,Z] look just like concatenation of the MSB and LSB registers for each axis respectively.
void	LIS3DH_update_temp(LIS3DH_HandleTypeDef *hLIS3DH);																				// Not used so far
float	LIS3DH_get_accels_rj(LIS3DH_HandleTypeDef *hLIS3DH, uint16_t * pdata);															// Get acceleration values in an array of Right Justified uint16_t types - you choose the array.
void	LIS3DH_get_gs(LIS3DH_HandleTypeDef *hLIS3DH, double * pdata);																	// Get acceleration values in g's, filled into a double array of your choosing

void 	LIS3DH_read(LIS3DH_HandleTypeDef *hLIS3DH, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout);		//
void 	LIS3DH_write(LIS3DH_HandleTypeDef *hLIS3DH, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout);
void 	LIS3DH_assert_spi_bus(LIS3DH_HandleTypeDef *hLIS3DH);


#ifdef __cplusplus
}
#endif

#endif /* LIS3DH_H */




