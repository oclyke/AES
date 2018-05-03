/* 
lidarlitev3.h

Written by Owen Lyke April 2018
Updated May 2018

A library to interface with the LidarLite V3 Lidar Rangefinder via I2C on the STM32F4 series microcontrollers
*/

#ifndef LIDARLITEV3_H
#define LIDARLITEV3_H

#include "stm32f4xx_hal.h"

 // I2C parameters of LIDARLITEV3
 // -----------------------------
#define LIDARLITEV3_I2C_ADDRESS_DEFAULT				0x62	// 7-bit I2C address, 8-bit address effectively 0xC4 write and 0xC5 read
#define LIDARLITEV3_I2C_FREQ_MAX					400000	// 400 kHz maximum I2C frequency
#define LIDARLITEV3_I2C_TIMEOUT_DEFAULT 			100U	// A default timeout to use

// ACQ_COMMAND
// -----------
//#define LIDARLITEV3_ACQ_COMMAND_DEFAULT 		0x00	// Power up default value // Note: no default value for this register
#define LIDARLITEV3_RESET 						0x00	// Reset device, all registers return to default alues
#define LIDARLITEV3_MEASURE_NO_BIAS_CORR		0x03	// Take distance measurement without receiver bias correction
#define LIDARLITEV3_MEASURE_W_BIAS_CORR			0x04 	// Take distance measurement with receiver bias correction

// SIG_COUNT_VAL
// -------------
#define LIDARLITEV3_SIG_COUNT_VAL_DEFAULT		0x80
// Limits the number of times the device will integrate acquisitions to find a correlation record peak which occurs at long rage or with low target reflectivity. 
// This controls the minimum measurement rate and maximum range. The unitless relationship is roughly as follows: rate = 1/n and range = n^(1/4) where n is the number of acquisitions

// ACQ_CONFIG_REG
// --------------
#define LIDARLITEV3_ACQ_CONFIG_REG_DEFAULT 		0x08	// Power up default value 
#define LIDARLITEV3_REFERENCE_PROCESS_DISABLE	0x40 	// Disable referece process during measurement
#define LIDARLITEV3_USE_MEASURE_DELAY			0x20	// Changes the delay between consecutive measurements from the default to the value shown in the MEASURE_DELAY register
#define LIDARLITEV3_REFERENCE_FILTER_DISABLE	0x10 	// Disable the reference filter
#define LIDARLITEV3_QUICK_TERMINATE				0x00	// Enables quick-termination of the device anticiaptes that signal peak will reach maximum value
#define LIDARLITEV3_QUICK_TERMINATE_DISABLE		0x08 	// Disables quick termination of the measurement
#define LIDARLITEV3_USE_REF_COUNT_VAL			0x04 	// Use reference acquisition count from REF_COUNT_VAL
#define LIDARLITEV3_PIN_FUNC_PWM				0x00	// Default PWM mode 
#define LIDARLITEV3_PIN_FUNC_STATUS				0x01 	// Status output mode, pin will be driven active high while busy
#define LIDARLITEV3_PIN_FUNC_PWM_FIXED_DELAY 	0x02 	// Fixed delay PWM mode. Pulling pin low will not trigger a measurement
#define LIDARLITEV3_PIN_FUNC_OSC_OUT			0x03 	// Nominal 31.25 kHz output. Accuracy of the silicon oscillator affects measuremnt accuracy, so this output can be used to measure the frequency and apply a compensation factor.

// THRESHOLD_BYPASS
// ----------------
#define LIDARLITEV3_THRESHOLD_BYPASS_DEFAULT 	0x00	// Power up default value 
#define LIDARLITEV3_NO_THRESHOLD_BYPASS			0x00	// Keeps defualt valid measurement detection algorithm
#define LIDARLITEV3_HIGH_SENS_THRESH			0x20	// Switches valid measurement detection to a simple threshold, with the 0x20 value for high sensitivity
#define LIDARLITEV3_LOW_SENS_THRESH				0x60	// Switches valid measurement detection to a simple threshold, with the 0x60 value for low sensitivity

// OUTER_LOOP_COUNT
// ----------------
#define LIDARLITEV3_OUTER_LOOP_COUNT_DEFAULT 	0x00	// Power up default value 

// REF_COUNT_VAL
// ----------------
#define LIDARLITEV3_REF_COUNT_VAL_DEFAULT 		0x00	// Power up default value 

// COMMAND
// ----------------
//#define LIDARLITEV3_COMMAND_DEFAULT 			0x00	// Power up default value 
#define LIDARLITEV3_TEST_MODE_DISBLE 			0x00	// Disable test mode
#define LIDARLITEV3_TEST_MODE_ENABLE			0x07	// Enable test mode 

// MEASURE_DELAY
// ----------------
#define LIDARLITEV3_MEASURE_DELAY_DEFAULT 		0x14	// Power up default value 

// POWER_CONTROL
// ----------------
#define LIDARLITEV3_POWER_CONTROL_DEFAULT 		0x80	// Power up default value 
#define LIDARLITEV3_DISABLE_RECEIVER			0x01	// Disables the receiver to save ~40 mA.
#define LIDARLITEV3_SLEEP						0x04	// Sleep mode saves ~20 mA.



// ==================================================
//                      Typdefs
// ==================================================
// Register Address Enum
// ---------------------
typedef enum {
	LIDARLITEV3_REG_ACQ_COMMAND 					= 0x00,
	LIDARLITEV3_REG_STATUS,
	LIDARLITEV3_REG_SIG_COUNT_VAL,
	// Reserved 0x03
	LIDARLITEV3_REG_ACQ_CONFIG_REG 					= 0x04,
	// Reserved 0x05 - 0x08
	LIDARLITEV3_REG_VELOCITY 						= 0x09,
	// Reserved 0x0A - 0x0B
	LIDARLITEV3_REG_PEAK_CORR						= 0x0C,
	LIDARLITEV3_REG_NOISE_PEAK,
	LIDARLITEV3_REG_SIGNAL_STRENGTH,
	LIDARLITEV3_REG_FULL_DELAY_HIGH,	
	LIDARLITEV3_REG_FULL_DELAY_LOW,
	LIDARLITEV3_REG_OUTER_LOOP_COUNT,
	LIDARLITEV3_REG_REF_COUNT_VAL,
	// Reserved 0x13
	LIDARLITEV3_REG_LAST_DELAY_HIGH		= 0x14,
	LIDARLITEV3_REG_LAST_DELAY_LOW,
	LIDARLITEV3_REG_UNIT_ID_HIGH,
	LIDARLITEV3_REG_UNIT_ID_LOW,
	LIDARLITEV3_REG_I2C_ID_HIGH,
	LIDARLITEV3_REG_I2C_ID_LOW,
	LIDARLITEV3_REG_I2C_SEC_ADDR,
	// Reserved 0x1B
	LIDARLITEV3_REG_THRESHOLD_BYPASS				= 0x1C,
	// Reserved 0x1D
	LIDARLITEV3_REG_I2C_CONFIG						= 0x1E,

	// Reserved 0x1F - 0x3F
	// External registers (advanced setup, must be supplied with bit 6 set)
	LIDARLITEV3_REG_COMMAND							= 0x40,
	// Reserved 0x41 - 0x44
	LIDARLITEV3_REG_MEASURE_DELAY	 				= 0x45,
	// Reserved 0x46 - 0x4B
	LIDARLITEV3_REG_PEAK_BCK	 					= 0x4C,
	// Reserved 0x4D - 0x51
	LIDARLITEV3_REG_CORR_DATA	 					= 0x52,
	LIDARLITEV3_REG_CORR_DATA_SIGN,
	// Reserved 0x54 - 0x5C
	LIDARLITEV3_REG_ACQ_DATA	 					= 0x5D,
	// Reserved 0x5E - 0x64
	LIDARLITEV3_REG_POWER_CONTROL	 				= 0x65
}LIDARLITEV3_RegisterTypeDef;


// Initialization Data
// -------------------
typedef struct{
	uint8_t 	SIG_COUNT_VAL_VAL;		// Value for RT_CFG, constructed by ORing appropriate defined values
	uint8_t 	ACQ_CONFIG_REG_VAL;		// Value for RT_THS, constructed by ORing appropriate defined values
	uint8_t 	THRESHOLD_BYPASS_VAL;		// Value for CTRL0_REG, constructed by ORing appropriate defined values
	uint8_t 	OUTER_LOOP_COUNT_VAL;		// Value for CTRL1_REG, constructed by ORing appropriate defined values
	uint8_t 	REF_COUNT_VAL_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
	uint8_t 	COMMAND_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
	uint8_t 	MEASURE_DELAY_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
	uint8_t 	POWER_CONTROL_VAL;		// Value for CTRL2_REG, constructed by ORing appropriate defined values
}LIDARLITEV3_InitTypeDef;

// Handle 
// ------------------
typedef struct{
	LIDARLITEV3_InitTypeDef  	Init;			// Initialization parameters for the sensor
	uint32_t					Timeout;		// I2C bus timeout

	I2C_HandleTypeDef			*hi2c;			// Which I2C bus is the sensor using?
	uint8_t						i2c_addr_7;		// 7 bit I2C address

	uint8_t						STATUS;			// System status
	uint16_t					D;				// Latest distance measurement
	uint8_t						V; 				// Difference between current and previous distance measurements. Use with knowledge of time between measurements to compute actual velocity
}LIDARLITEV3_HandleTypeDef;


#define LIDARLITEV3_wdb_num_bytes		0xFF
uint8_t LIDARLITEV3_write_data_buffer[LIDARLITEV3_wdb_num_bytes];




// ==================================================
//                      Functions
// ==================================================

void 	LIDARLITEV3_init(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3);
void 	LIDARLITEV3_init_continuous_measurement(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t DELAY, uint8_t MEASURE_TYPE);
void	LIDARLITEV3_reset(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3);

void 	LIDARLITEV3_update_vals(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3);

void 	LIDARLITEV3_read(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t reg_add, uint8_t *data_out_ptr, uint8_t num_reads, uint32_t timeout);		//
void 	LIDARLITEV3_write(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3, uint8_t reg_add, uint8_t *data_in_ptr, uint8_t num_writes, uint32_t timeout);
void 	LIDARLITEV3_assert_i2c_bus(LIDARLITEV3_HandleTypeDef *hLIDARLITEV3);





#endif /* LIDARLITEV#_H */


