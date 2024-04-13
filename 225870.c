uchar Utf8::CalculateValue(const byte* str,
                           unsigned length,
                           unsigned* cursor) {
  // We only get called for non-ASCII characters.
  if (length == 1) {
    *cursor += 1;
    return kBadChar;
  }
  byte first = str[0];
  byte second = str[1] ^ 0x80;
  if (second & 0xC0) {
    *cursor += 1;
    return kBadChar;
  }
  if (first < 0xE0) {
    if (first < 0xC0) {
      *cursor += 1;
      return kBadChar;
    }
    uchar code_point = ((first << 6) | second) & kMaxTwoByteChar;
    if (code_point <= kMaxOneByteChar) {
      *cursor += 1;
      return kBadChar;
    }
    *cursor += 2;
    return code_point;
  }
  if (length == 2) {
    *cursor += 1;
    return kBadChar;
  }
  byte third = str[2] ^ 0x80;
  if (third & 0xC0) {
    *cursor += 1;
    return kBadChar;
  }
  if (first < 0xF0) {
    uchar code_point = ((((first << 6) | second) << 6) | third)
        & kMaxThreeByteChar;
    if (code_point <= kMaxTwoByteChar) {
      *cursor += 1;
      return kBadChar;
    }
    *cursor += 3;
    return code_point;
  }
  if (length == 3) {
    *cursor += 1;
    return kBadChar;
  }
  byte fourth = str[3] ^ 0x80;
  if (fourth & 0xC0) {
    *cursor += 1;
    return kBadChar;
  }
  if (first < 0xF8) {
    uchar code_point = (((((first << 6 | second) << 6) | third) << 6) | fourth)
        & kMaxFourByteChar;
    if (code_point <= kMaxThreeByteChar) {
      *cursor += 1;
      return kBadChar;
    }
    *cursor += 4;
    return code_point;
  }
  *cursor += 1;
  return kBadChar;
}