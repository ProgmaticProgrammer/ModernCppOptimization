/*
 * cmain.c
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#include <stdint.h>
#include "i2c.h"

#define OLED_I2C_ADDRESS ((uint32_t *)0x01234567)

static int oled_write(uint8_t *bytes, int count) {
  int tx = i2c_master_tx(OLED_I2C_ADDRESS, bytes, count);
  if (tx < 1) return -1;
  return tx;
}

int main() {
  oled_write((uint8_t[]){0x00, 0xAF}, 2);
  oled_write((uint8_t[]){0x00, 0x8D, 0x14}, 3);
  oled_write((uint8_t[]){0x00, 0xA8, 0x0F}, 3);
  oled_write((uint8_t[]){0x00, 0xDA, 0x02}, 3);
  oled_write((uint8_t[]){0x00, 0x81, 0xFF}, 3);
  oled_write((uint8_t[]){0x00, 0x82, 0xFF}, 3);
  oled_write((uint8_t[]){0x00, 0x83, 0xFF}, 3);

  return 0;
}
