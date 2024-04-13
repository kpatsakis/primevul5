uchar Utf8::ValueOf(const byte* bytes, unsigned length, unsigned* cursor) {
  if (length <= 0) return kBadChar;
  byte first = bytes[0];
  // Characters between 0000 and 0007F are encoded as a single character
  if (first <= kMaxOneByteChar) {
    *cursor += 1;
    return first;
  }
  return CalculateValue(bytes, length, cursor);
}