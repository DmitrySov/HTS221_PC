/*
 * modbus.c
 *
 * Created: 04.03.2020 16:09:30
 *  Author: ivandk
 */ 


/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>

#include "modbus.h"


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/


/***********************************************************************************
 * Function:		modbus_crc16
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Рассчитывает контрольную сумму CRC-16 для обмена данными по 
 *					протоколу Modbus. Полином, используемый при расчете:
 *					(x15 + x13 + x0).
 *
 * parameters:		uint8_t *buffer				указатель на массив данных;
 *					uint8_t n					размер массива (число байт);
 * 
 * on return:		uint16_t s					контрольная сумма CRC-16;
 *
 *************************************************************************************/

uint16_t modbus_crc16(uint8_t *buffer, uint8_t n)
{
	uint16_t s = 0xFFFF;							//	начальное значение CRC-16;
	uint8_t j;
	while (n)
	{
		s ^= *buffer;
		for (j = 0; j < 8; j++)
		{
			if (s & 0x0001)									//	"если разряд, сдвинутый вправо, равен 1, ...";
			{
				s >>= 1;
				s ^= 0xA001;
			}
			else
			{
				s >>= 1;
			}
		}
		buffer++;
		n--;
	}
	return s;
}

/***********************************************************************************
 * Function:		modbus_crc16_tx
 *
 *----------------------------------------------------------------------------------
 *
 * description:		
 *
 * parameters:		
 * 
 * on return:		
 *
 *************************************************************************************/

void modbus_crc16_tx(uint8_t *buffer, uint8_t n)
{
	uint16_t s;
	s = modbus_crc16(buffer, n);
	*(buffer + n) = (uint8_t)s;							//	младший байт контрольной суммы;
	*(buffer + n + 1) = (uint8_t)(s >> 8);				//	старший байт ...;
}

/***********************************************************************************
 * Function:		modbus_swap 
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Переставляет местами каждые два байта в массиве данных. Необходима
 *					для правильной передачи.
 *
 * parameters:		uint8_t *buffer_to				указатель на буфер-источник (куда);
					uint8_t *buffer_from			указатель на буфер приемник (откуда);
 *					uint8_t n						количество 16-ти разрядных регистров;
 * 
 * on return:		-
 *
 *************************************************************************************/

void modbus_swap(uint8_t *buffer_to, uint8_t *buffer_from, uint8_t n)
{
	uint8_t a, b;
	while (n)
	{
		a = *(buffer_from + 0);
		b = *(buffer_from + 1);
		*(buffer_to + 0) = b;
		*(buffer_to + 1) = a;
		buffer_to += 2;
		buffer_from += 2;
		n--;
	}
}
