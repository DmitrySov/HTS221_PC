/*
 * modbus_slave.h
 *
 * Created: 04.03.2020 18:13:28
 *  Author: ivandk
 */ 


#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/

#define MODBUS_SLAVE_BUF_SIZE	74									//	число байт, занимаемых регистрами;


/* Пользовательские функции Modbus */

#define MODBUS_SLAVE_0X46		0x46
#define MODBUS_SLAVE_0X37		0x37


/*_____ D E C L A R A T I O N ______________________________________________*/

void modbus_slave_illegal(modbus_frame_t *frame, uint8_t illegal_code);

void modbus_slave(modbus_frame_t *frame);


#endif /* MODBUS_SLAVE_H_ */