#include "endianness_change.h"

#include <stdint.h>
uint32_t endianness_change_little(uint32_t data, uint8_t len) {
  uint8_t* p_d  = (uint8_t*)&data;
  uint32_t temp = 0;
  for (int i = 0; i < len; i++) {
    temp |= (p_d[i]) << 8 * (len - i - 1);
  }
  return temp;
}