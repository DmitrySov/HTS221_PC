/*
 * modbus_slave_0x10.c
 *
 * Created: 31.03.2020 0:06:18
 *  Author: User
 */ 


/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>
#include <string.h>

#include "main.h"
#include "usart0.h"
#include "modbus_slave.h"
#include "modbus_slave_0x10.h"
#include "modbus_slave_map.h"


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/

extern irvis_passport_t passport;

/***********************************************************************************
 * Function:		modbus_slave_x10	
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

void modbus_slave_x10(modbus_frame_t *frame)
{
	uint16_t addr;																//	адрес первого регистра для записи;
	uint16_t n;																	//	количество регистров для записи;
	uint16_t s;																	//	переменная для содержимого регистров;
	uint8_t *p;																	//	указатель на данные для записи в регистры;
	uint8_t i;																	//	статус записи;
	uint8_t j;																	//	промежуточная переменная для указателя на данные;

	/*	Широковещательный запрос поддерживается	*/

	if (frame -> n != (*(frame -> buffer + 6) + 9))								//	"если количество принятых байт не совпадает
	{																			//	с полученным числом, ..."
		modbus_slave_illegal(frame, MODBUS_ILLEGAL_FUNCTION);						//	ошибка - "Функция не поддерживается";
		return;
	}

	addr = (uint16_t)(*(frame -> buffer + 2));									//	сохраняем адрес первого регистра;
	addr <<= 8; addr &= 0xFF00;
	addr |= *(frame -> buffer + 3);

	n = (uint16_t)(*(frame -> buffer + 4));										//	сохраняем количество регистров для записи;
	n <<= 8; n &= 0xFF00;
	n |= *(frame -> buffer + 5);
	if (n > 64)																	//	"если количество регистров превышает максимальное, ...";
	{
		modbus_slave_illegal(frame, MODBUS_ILLEGAL_DATA_VALUE);						//	ошибка - "Неверный формат данных";
		return;
	}

	p = frame -> buffer; p += 7;												//	сохраняем указатель, откуда брать данные;

	i = 0;																		//	записываем блок данных;
	while (n)
	{
		j = *(p + 0);																//	сохраняем значение для записи в регистр (2 байта);
		s = (uint16_t)j;
		s <<= 8; s &= 0xFF00;
		j = *(p + 1);
		s |= j;
		i |= modbus_slave_x10_write(addr, s);										//	записываем значение в память;
		p += 2;																		//	переходим к следующему регистру;
		addr++;
		n--;
	}
		
	if (i)																			//	"если произошла запись по несуществующему адресу, ...";
	{
		modbus_slave_illegal(frame, MODBUS_ILLEGAL_DATA_ADDRESS);						//	ошибка - "Неверный адрес данных";
	}
	else
	{
		if (*(frame -> buffer + 0))														//	"если адрес не равен 0, ...";
		{
			modbus_crc16_tx(frame -> buffer, 6);											//	добавляем контрольную сумму;
			usart0_transmit(frame -> buffer, 8);											//	отправляем ответ;
		}
	}
}

/***********************************************************************************
 * Function:		modbus_slave_x10_write	
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

uint8_t modbus_slave_x10_write(uint16_t address, uint16_t reg)
{
	uint8_t i = 1;
	
 	if (address == MODBUS_MAP_DEVICE_NUM)
 	{
	 	passport.device_num = reg;
	 	i = 0;
 	}

	return i;
}
