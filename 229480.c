UnicodeString::getChar32Start(int32_t offset) const {
  if((uint32_t)offset < (uint32_t)length()) {
    const UChar *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    return offset;
  } else {
    return 0;
  }
}