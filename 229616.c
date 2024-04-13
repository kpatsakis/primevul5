UnicodeString::replace(int32_t start,
               int32_t _length,
               UChar32 srcChar) {
  UChar buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = FALSE;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  return doReplace(start, _length, buffer, 0, isError ? 0 : count);
}