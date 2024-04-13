char *uint32tochar4(uint32_t lnum, char *data) {
  data[0] = (lnum >> 24) & 0xff;
  data[1] = (lnum >> 16) & 0xff;
  data[2] = (lnum >> 8) & 0xff;
  data[3] = (lnum >> 0) & 0xff;
  return data;
}