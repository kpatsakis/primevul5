UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  UChar buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = FALSE;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  return !isError && str.doAppend(buffer, 0, cLength).isWritable();
}