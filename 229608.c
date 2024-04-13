UnicodeString::toUTF8(int32_t start, int32_t len,
                      char *target, int32_t capacity) const {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     NULL,    // Don't care about number of substitutions.
                     &errorCode);
  return length8;
}