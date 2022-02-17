/*
 * HTS221.h
 *
 *  Created on: Aug 4, 2021
 *      Author: igur
 */

#ifndef INC_HTS221_H_
#define INC_HTS221_H_

#include "main.h"
#include "stdbool.h"

#define DEV_ADDR 			0x5F
#define I2C_HTS_TIMEOUT 	1000


// SAMPLE MODES
#define TEMP 0
#define HUMIDITY 1

#define TEMP_2				0
#define TEMP_4				1
#define TEMP_8				2
#define TEMP_16				3
#define TEMP_32				4
#define TEMP_64				5
#define TEMP_128			6
#define TEMP_256			7

#define HUM_4				0
#define HUM_8				1
#define HUM_16				2
#define HUM_32				3
#define HUM_64				4
#define HUM_128				5
#define HUM_256				6
#define HUM_512				7
// OUTPUT DATA RATES
#define ONESHOT				0
#define Hz_1				1
#define Hz_7				2
#define Hz_12_5				3
// REGISTERS
#define WHO_AM_I 			0x0F
#define AV_CONF 			0x10
#define CTRL_REG1			0x20
#define CTRL_REG2			0x21
#define CTRL_REG3			0x22
// OUTPUT REGISTERS
#define H_OUT				0x28
#define T_OUT				0x2A
// CALIBRATIOn REGISTERS
#define H0_rH_x2			0x30
#define H1_rH_x2			0x31
#define T0_degC_x8			0x32
#define T1_degC_x8			0x33
#define T1_T0_MSB			0x35
#define	H0_T0_OUT			0x36
#define H1_T0_OUT			0x3A
#define T0_OUT				0x3C
#define T1_OUT 				0x3E
#define T0_T1_MSB			0x35

// DRDY Signal properties
#define PUSH_PULL 0
#define OPEN_DRAIN 1
#define ACTIVE_HIGH 0
#define ACTIVE_LOW 1


typedef struct hts221_t
{
	I2C_HandleTypeDef* 	i2c_handler;
	uint8_t 			address;

	// Temperature coefficients
	uint8_t				T0_degc_x8;
	uint8_t				T1_degc_x8;
	int16_t				T0_out;
	int16_t				T1_out;
	uint16_t			T0_MSB;
	uint16_t			T1_MSB;

	// Humidity coefficients
	uint8_t 			H0_rh_x2;
	uint8_t				H1_rh_x2;
	int16_t				H0_T0_out;
	int16_t				H1_T0_out;


	int16_t				H0_rh;
	int16_t				H1_rh;
	uint16_t			T0_degc;
	uint16_t			T1_degc;

}hts221_t;


uint8_t HTS221_Init(hts221_t* sensor, I2C_HandleTypeDef* hi2c,  uint8_t Address);
void HTS221_set_temp_sample(hts221_t* sensor, uint8_t sample_mode);
void HTS221_set_humidity_sample(hts221_t* sensor, uint8_t sample_mode);
void HTS221_set_power_mode(hts221_t* sensor, bool mode);
void HTS221_set_output_data_rate(hts221_t* sensor, uint8_t data_rate);
void HTS221_get_data(hts221_t* sensor, int16_t* temperature_value, uint16_t* humidity_val);
void HTS221_get_data_OneHot(hts221_t* sensor, int16_t* temperature_value, uint16_t* humidity_val);
void HTS221_DRDY_Enable(hts221_t* sensor, uint8_t PP_OD, uint8_t DRDY_H_L);


#endif /* INC_HTS221_H_ */
