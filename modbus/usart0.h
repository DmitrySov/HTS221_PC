/*
 * usart0.h
 *
 * Created: 02.03.2020 16:53:26
 *  Author: ivandk
 */ 


#ifndef USART0_H_
#define USART0_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>
#include <stdbool.h>
#include <main.h>
#include "modbus.h"

/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/

#define RS485_RTS	IOPORT_CREATE_PIN(PIOA, 31)					//	выводы для передатчика RS-485;
#define RS485_RX	IOPORT_CREATE_PIN(PIOA, 5)
#define RS485_TX	IOPORT_CREATE_PIN(PIOA, 6)


/*_____ D E C L A R A T I O N ______________________________________________*/

void usart0_init(void);

volatile bool usart0_modbus_timeout_get(void);

modbus_frame_t usart0_modbus_frame_get(void);

void usart0_transmit(uint8_t *buffer, uint8_t n);


#endif /* USART0_H_ */
