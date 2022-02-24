/*
 * modbus_slave_0x03.c
 *
 * Created: 30.03.2020 23:07:53
 *  Author: User
 */ 


/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>
#include <string.h>

#include "main.h"
#include "usart0.h"
#include "modbus_slave.h"
#include "modbus_slave_0x03.h"
#include "modbus_slave_map.h"


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/

extern irvis_passport_t passport;

/***********************************************************************************
 * Function:		modbus_slave_x03	
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

void modbus_slave_x03(modbus_frame_t *frame)
{
	uint16_t addr;																//	адрес первого регистра для чтения;
	uint16_t n;																	//	количество регистров для чтения;
	uint16_t s;																	//	переменная для содержимого регистров;
	uint8_t *p;																	//	указатель для записи ответа;

	if (*(frame -> buffer + 0) == 0x00)											//	"если адрес устройства 0, ...";
	{
		return;																		//	ничего не отвечаем;
	}
	
	if (frame -> n != 8)														//	"если количество принятых байт не равно 8, ...";
	{
		modbus_slave_illegal(frame, MODBUS_ILLEGAL_FUNCTION);						//	ошибка - "Функция не поддерживается";
		return;
	}

	addr = (uint16_t)(*(frame -> buffer + 2));									//	сохраняем адрес первого регистра;
	addr <<= 8; addr &= 0xFF00;
	addr |= *(frame -> buffer + 3);

	n = (uint16_t)(*(frame -> buffer + 4));										//	сохраняем количество регистров для чтения;
	n <<= 8; n &= 0xFF00;
	n |= *(frame -> buffer + 5);
	if (n > ((MODBUS_SLAVE_BUF_SIZE - 9) / 2))									//	"если количество регистров превышает имеющееся, ...";
	{
		modbus_slave_illegal(frame, MODBUS_ILLEGAL_DATA_VALUE);						//	ошибка - "Неверный формат данных";
		return;
	}
	*(frame -> buffer + 2) = (uint8_t)(n << 1);									//	сохраняем количество байт в буфер для ответа;

	p = frame -> buffer + 3;													//	сохраняем указатель, куда помещать данные;

	while (n--)
	{
		s = modbus_slave_x03_read(addr++);											//	извлекаем данные по адресу в карте регистров;
		*p++ = (uint8_t)(s >> 8);													//	записываем во фрейм для передачи (старший байт);
		*p++ = (uint8_t)s;															//	... (младший байт);
	}

	modbus_crc16_tx(frame -> buffer, *(frame -> buffer + 2) + 3);				//	добавляем контрольную сумму;
	usart0_transmit(frame -> buffer, *(frame -> buffer + 2) + 5);				//	отправляем ответ;		
}

/***********************************************************************************
 * Function:		modbus_slave_x03_read	
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

uint16_t modbus_slave_x03_read(uint16_t address)
{
	uint16_t s = 0;																//	переменная для содержимого регистра;
	uint8_t *p;																	//	указатель на данные для регистра;
	
	if (address <= (MODBUS_MAP_PASSPORT + 7))
	{
		p = (uint8_t*)&passport;																//	"Паспорт прибора";
	}
	else
	{
		p = (uint8_t*)&s;
	}

	memcpy((uint8_t*)&s, p + (address * 2), sizeof(uint16_t));					//	сохраняем содержимое регистра по указателю;
	
	return s;
}
