void clear(uint8_t *dest, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    *(dest + i) = 0;
  }
}