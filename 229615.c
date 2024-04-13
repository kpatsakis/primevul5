UnicodeString::getChar32Limit(int32_t offset) const {
  int32_t len = length();
  if((uint32_t)offset < (uint32_t)len) {
    const UChar *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    return offset;
  } else {
    return len;
  }
}