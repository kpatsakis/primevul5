UnicodeString::setTo(UChar *buffer,
                     int32_t buffLength,
                     int32_t buffCapacity) {
  if(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    return *this;
  }

  if(buffer == NULL) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    return *this;
  }

  if(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    return *this;
  } else if(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    const UChar *p = buffer, *limit = buffer + buffCapacity;
    while(p != limit && *p != 0) {
      ++p;
    }
    buffLength = (int32_t)(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  return *this;
}