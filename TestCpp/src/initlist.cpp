#include <cstdint>
#include <initializer_list>

extern "C" {
#include "i2c.h"
}

const auto OLED_I2C_ADDRESS = reinterpret_cast<uint32_t*>(0x01234567);

int oled_write(uint8_t const* bytes, int count) {
  int tx = i2c_master_tx(OLED_I2C_ADDRESS, const_cast<uint8_t*>(bytes), count);
  if (tx < 1) return -1;
  return tx;
}

int oled_write(std::initializer_list<uint8_t> bytes) {
  return oled_write(bytes.begin(), bytes.size());
}

int initlist_main() {
  oled_write({0x00, 0xAF});
  oled_write({0x00, 0x8D, 0x14});
  oled_write({0x00, 0xA8, 0x0F});
  oled_write({0x00, 0xDA, 0x02});
  oled_write({0x00, 0x81, 0xFF});
  oled_write({0x00, 0x82, 0xFF});
  oled_write({0x00, 0x83, 0xFF});
  return 0;
}
