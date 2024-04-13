int cmp(const uint8_t *value1, const uint8_t *value2, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    if (*(value1+i) != *(value2+i)) {
      return -1;
    }
  }
  return 0;
}