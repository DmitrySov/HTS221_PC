/*
 * modbus_slave_0x03.h
 *
 * Created: 30.03.2020 23:08:12
 *  Author: User
 */ 


#ifndef MODBUS_SLAVE_0X03_H_
#define MODBUS_SLAVE_0X03_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include <stdint.h>


/*_____ M A C R O S ________________________________________________________*/



/*_____ D E F I N I T I O N ________________________________________________*/



/*_____ D E C L A R A T I O N ______________________________________________*/

void modbus_slave_x03(modbus_frame_t *frame);

uint16_t modbus_slave_x03_read(uint16_t address);


#endif /* MODBUS_SLAVE_0X03_H_ */