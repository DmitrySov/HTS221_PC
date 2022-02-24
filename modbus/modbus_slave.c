/*
 * modbus_slave.c
 *
 * Created: 04.03.2020 18:13:13
 *  Author: ivandk
 */ 


/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>

//#include "lib/usart0/usart0.h"
#include "modbus.h"
#include "modbus_slave.h"
#include "modbus_slave_0x03.h"
#include "modbus_slave_0x10.h"


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/


/***********************************************************************************
 * Function:		modbus_slave_illegal 
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Отправляет ответ при получении неправильного запроса по протоколу
 *					Modbus.
 *
 * parameters:		
 * 
 * on return:		-
 *
 *************************************************************************************/

void modbus_slave_illegal(modbus_frame_t *frame, uint8_t illegal_code)
{
	if (*(frame -> buffer + 0))											//	"если был запрос по адресу, ...";
	{
		*(frame -> buffer + 1) |= 0x80;										//	модификация кода функции;
		*(frame -> buffer + 2) = illegal_code;								//	добавляем код ошибки;
		modbus_crc16_tx(frame -> buffer, 3);								//	добавляем контрольную сумму;
		usart0_transmit(frame -> buffer, 5);								//	передача ответа;
	}
}

/***********************************************************************************
 * Function:		modbus_slave	
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Обрабатывает данные, полученные по Modbus.
 *
 * parameters:		
 * 
 * on return:		
 *
 *************************************************************************************/

void modbus_slave(modbus_frame_t *frame)
{
	if (modbus_crc16(frame -> buffer, frame -> n))								//	проверка контрольной суммы;
	{
		;																				//	ошибка CRC-16;
	}
	else
	{
		if ((*(frame -> buffer + 0) == 1) || (*(frame -> buffer + 0) == 0))				//	проверка адреса;
		{
			switch (*(frame -> buffer + 1))
			{
				case 0x03:
				modbus_slave_x03(frame);														//	чтение регистров;
				break;

				case 0x10:
				modbus_slave_x10(frame);														//	запись регистров;
				break;

				//	case 0x11:
				//	modbus_slave_x11(frame);													//	ID устройства;
				//	break;
				
				//	case 0x37:
				//	modbus_slave_x37(frame);													//	Инкапсуляция;
				//	break;

				default:
				modbus_slave_illegal(frame, MODBUS_ILLEGAL_FUNCTION);							//	обработка неправильного запроса;
				break;
			}
		}
		else
		{
			;																				//	запрос по другому адресу;
		}
	}
}
