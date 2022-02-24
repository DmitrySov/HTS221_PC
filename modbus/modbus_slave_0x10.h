/*
 * modbus_slave_0x10.h
 *
 * Created: 31.03.2020 0:06:45
 *  Author: User
 */ 


#ifndef MODBUS_SLAVE_0X10_H_
#define MODBUS_SLAVE_0X10_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/

void modbus_slave_x10(modbus_frame_t *frame);

uint8_t modbus_slave_x10_write(uint16_t address, uint16_t reg);


#endif /* MODBUS_SLAVE_0X10_H_ */