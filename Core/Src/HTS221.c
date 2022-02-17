/*
 * HTS221.c
 *
 *  Created on: Aug 4, 2021
 *      Author: igur
 */


#include "HTS221.h"


static uint8_t read8(hts221_t* sensor, uint8_t Mem_reg)
{
	uint8_t value;
	HAL_I2C_Mem_Read(sensor->i2c_handler, sensor->address, Mem_reg, 1, &value, 1, I2C_HTS_TIMEOUT);

	return value;
}

static uint16_t read16(hts221_t* sensor, uint8_t Mem_reg)
{
	uint8_t value[2];

	HAL_I2C_Mem_Read(sensor->i2c_handler, sensor->address, (Mem_reg | 0x80), I2C_MEMADD_SIZE_8BIT, value, 2, I2C_HTS_TIMEOUT);

	return (((uint16_t)(value[1])) << 8) | (uint16_t)value[0];
}

static void write8(hts221_t* sensor, uint8_t Mem_reg, uint8_t* data)
{
	HAL_I2C_Mem_Write(sensor->i2c_handler, sensor->address, Mem_reg, 1, data, 1, I2C_HTS_TIMEOUT);
}


/////////////////////////////////////////////


void HTS221_set_power_mode(hts221_t* sensor, bool mode)
{
	// mode 1 - Active,  0 - Power down
	uint8_t tmp;
	tmp = read8(sensor, CTRL_REG1);

	if(mode == 0) tmp &= 0x7F; // 0xxx xxxx
	else
	{
		tmp |= 0x80;
	}

	write8(sensor, CTRL_REG1, &tmp);
}



void HTS221_set_output_data_rate(hts221_t* sensor, uint8_t data_rate)
{
	if(data_rate > 3) data_rate = 3;

	uint8_t tmp;
	tmp = read8(sensor, CTRL_REG1);
	tmp &= 0xFC; // XXXX XX00
	tmp |= data_rate & 0x03; // double protection for writing wrong number

	write8(sensor, CTRL_REG1, &tmp);
}



void HTS221_set_temp_sample(hts221_t* sensor, uint8_t sample_mode)
{
	if(sample_mode > 7) sample_mode = 7;

	uint8_t tmp;
	tmp = read8(sensor, AV_CONF);

	tmp &= 0xC7; // 0bXX00 0xxx
	tmp |= (sample_mode) << 3;

	write8(sensor, AV_CONF, &tmp);
}

void HTS221_set_humidity_sample(hts221_t* sensor, uint8_t sample_mode)
{
	if(sample_mode > 7) sample_mode = 7;

	uint8_t tmp;
	tmp = read8(sensor, AV_CONF);
	tmp &= 0xF8; // 0bXXXXX000

	tmp |= (sample_mode) & 0x07; // protection for writing number bigger than 7

	write8(sensor, AV_CONF, &tmp);

}


static void HTS221_start_onehot_conversion(hts221_t* sensor)
{
	uint8_t tmp;
	tmp = read8(sensor, CTRL_REG2);

	tmp |= 0x01; // write 1 in first bit
	write8(sensor, CTRL_REG2, &tmp);
}


uint8_t HTS221_Init(hts221_t* sensor, I2C_HandleTypeDef* hi2c,  uint8_t Address)
{
	sensor->i2c_handler = hi2c;
	sensor->address = (Address << 1);

	// check sensor ID
	uint8_t sensor_id;
	sensor_id = read8(sensor, WHO_AM_I);
	if(sensor_id != 0xBC)
	{
		return 1;
	}

	// read calibration coefficients
	// 1. Temperature
	sensor->T0_degc_x8 = read8(sensor, T0_degC_x8);
	sensor->T1_degc_x8 = read8(sensor, T1_degC_x8);
	sensor->T0_out = (int16_t)read16(sensor, T0_OUT);
	sensor->T1_out = (int16_t)read16(sensor, T1_OUT);

	sensor->T0_MSB = (read8(sensor, T0_T1_MSB) & 0x3) << 8;
	sensor->T1_MSB = (read8(sensor, T0_T1_MSB) & 0x0C) << 6;

	sensor->T1_degc = ((sensor->T1_MSB) | (uint16_t)(sensor->T1_degc_x8)) >> 3;
	sensor->T0_degc = ((sensor->T0_MSB) | (uint16_t)(sensor->T0_degc_x8)) >> 3;

	// 2. Humidity
	sensor->H0_rh_x2 = read8(sensor, H0_rH_x2);
	sensor->H1_rh_x2 = read8(sensor, H1_rH_x2);
	sensor->H0_T0_out = (int16_t)read16(sensor, H0_T0_OUT);
	sensor->H1_T0_out = (int16_t)read16(sensor, H1_T0_OUT);
	sensor->H0_rh = (sensor->H0_rh_x2) >> 1;
	sensor->H1_rh = (sensor->H1_rh_x2) >> 1;


	// set sample mode
	HTS221_set_temp_sample(sensor, TEMP_256);
	HTS221_set_humidity_sample(sensor, HUM_512);
	// set output data rate
	HTS221_set_output_data_rate(sensor, ONESHOT);
	// Turn on the device
 	HTS221_set_power_mode(sensor, 1);
	return 0;
}


void HTS221_get_data(hts221_t* sensor, int16_t* temperature_value, uint16_t* humidity_val)
{
	int32_t tmp_t, tmp_h;
	int16_t h_out, t_out;

	// wait for end of conversion

	t_out = read16(sensor, T_OUT);
	h_out = read16(sensor, H_OUT);

	// humidity calculations
	tmp_h = ((int32_t)(h_out - sensor->H0_T0_out)) * ((int32_t)(sensor->H1_rh - sensor->H0_rh));
	*humidity_val =  (uint16_t)(tmp_h/(sensor->H1_T0_out - sensor->H0_T0_out) + sensor->H0_rh);

	// temperature calculations
	tmp_t = ((int32_t)(t_out - sensor->T0_out)) * ((int32_t)(sensor->T1_degc - sensor->T0_degc));
	*temperature_value = tmp_t /(sensor->T1_out - sensor->T0_out) + sensor->T0_degc;
}



void HTS221_get_data_OneHot(hts221_t* sensor, int16_t* temperature_value, uint16_t* humidity_val)
{
	HTS221_start_onehot_conversion(sensor);
	HTS221_get_data(sensor, temperature_value, humidity_val);
}


void HTS221_DRDY_Enable(hts221_t* sensor, uint8_t PP_OD, uint8_t DRDY_H_L)
{
	uint8_t tmp;
	tmp = read8(sensor, CTRL_REG3);

	// clear byte
	tmp &= 0x3B;  // 0b00XXX0XX

	// Enable DRDY pin
	tmp |= 0x04;

	// Set output type 0 - push pull, 1 - open drain
	if(PP_OD == OPEN_DRAIN)
	{
		tmp |= 0x40;
	}

	if(DRDY_H_L == ACTIVE_LOW)
	{
		tmp |= 0x80;
	}

	write8(sensor, CTRL_REG3, &tmp);

}

