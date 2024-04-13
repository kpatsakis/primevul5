unsigned Utf8::EncodeOneByte(char* str, uint8_t c) {
  static const int kMask = ~(1 << 6);
  if (c <= kMaxOneByteChar) {
    str[0] = c;
    return 1;
  }
  str[0] = 0xC0 | (c >> 6);
  str[1] = 0x80 | (c & kMask);
  return 2;
}