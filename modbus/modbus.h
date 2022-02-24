/*
 * modbus.h
 *
 * Created: 04.03.2020 16:09:46
 *  Author: ivandk
 */ 


#ifndef MODBUS_H_
#define MODBUS_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/

/*	Структура для кадра Modbus	*/

typedef struct {
	uint8_t *buffer;								//	указатель на данные;
	uint8_t n;										//	количество байт данных в принятом кадре;
} modbus_frame_t;


/* Функции Modbus */

#define MODBUS_READ_HOLDING_REGISTERS			0x03				//	чтение одного / нескольких последовательных регистров;
#define MODBUS_LOOPBACK_DIAGNOSTIC_TEST			0x08				//	диагностическая функция;
#define MODBUS_FORCE_MULTIPLE_REGISTERS			0x10				//	запись одного / нескольких регистров;
#define MODBUS_REPORT_SLAVE_ID					0x11				//	вывод информации об адресуемом устройстве;

/* Коды ошибок Modbus */

#define MODBUS_ILLEGAL_FUNCTION					0x01
#define MODBUS_ILLEGAL_DATA_ADDRESS				0x02
#define MODBUS_ILLEGAL_DATA_VALUE				0x03
#define MODBUS_FAILURE_IN_ASSOCIATED_DEVICE 	0x04
#define MODBUS_ACKNOWLEDGE						0x05
#define MODBUS_BUSY_REJECTED_MESSAGE			0x06
#define MODBUS_NAK_NEGATIVE_ACKNOWLEDGMENT 		0x07


/*_____ D E C L A R A T I O N ______________________________________________*/

uint16_t modbus_crc16(uint8_t *buffer, uint8_t n);

void modbus_crc16_tx(uint8_t *buffer, uint8_t n);

void modbus_swap(uint8_t *buffer_to, uint8_t *buffer_from, uint8_t n);


#endif /* MODBUS_H_ */