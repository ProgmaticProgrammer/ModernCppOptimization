/*
 * i2c.h
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

extern int i2c_master_tx(uint32_t *addr, uint8_t *data, int count);


#endif /* I2C_H_ */
