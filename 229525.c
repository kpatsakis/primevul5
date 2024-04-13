UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  if(count <= 0 || (uint32_t)c > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } else if(c <= 0xffff) {
    int32_t length = count;
    if(capacity < length) {
      capacity = length;
    }
    if(allocate(capacity)) {
      UChar *array = getArrayStart();
      UChar unit = (UChar)c;
      for(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } else {  // supplementary code point, write surrogate pairs
    if(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      return;
    }
    int32_t length = count * 2;
    if(capacity < length) {
      capacity = length;
    }
    if(allocate(capacity)) {
      UChar *array = getArrayStart();
      UChar lead = U16_LEAD(c);
      UChar trail = U16_TRAIL(c);
      for(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}