UnicodeString::doAppend(const UChar *srcChars, int32_t srcStart, int32_t srcLength) {
  if(!isWritable() || srcLength == 0 || srcChars == NULL) {
    return *this;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  if(srcLength < 0) {
    // get the srcLength if necessary
    if((srcLength = u_strlen(srcChars)) == 0) {
      return *this;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;
  if (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
    setToBogus();
    return *this;
  }

  // Check for append onto ourself
  const UChar* oldArray = getArrayStart();
  if (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    if (copy.isBogus()) {
      setToBogus();
      return *this;
    }
    return doAppend(copy.getArrayStart(), 0, srcLength);
  }

  // optimize append() onto a large-enough, owned string
  if((newLength <= getCapacity() && isBufferWritable()) ||
      cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
    UChar *newArray = getArrayStart();
    // Do not copy characters when
    //   UChar *buffer=str.getAppendBuffer(...);
    // is followed by
    //   str.append(buffer, length);
    // or
    //   str.appendString(buffer, length)
    // or similar.
    if(srcChars != newArray + oldLength) {
      us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
    }
    setLength(newLength);
  }
  return *this;
}