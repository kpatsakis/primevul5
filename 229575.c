UnicodeString::UnicodeString(UChar *buff,
                             int32_t buffLength,
                             int32_t buffCapacity) {
  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  if(buff == NULL) {
    // treat as an empty string, do not alias
    setToEmpty();
  } else if(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } else {
    if(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      const UChar *p = buff, *limit = buff + buffCapacity;
      while(p != limit && *p != 0) {
        ++p;
      }
      buffLength = (int32_t)(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}