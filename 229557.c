UnicodeString::doReplace(int32_t start,
             int32_t length,
             const UChar *srcChars,
             int32_t srcStart,
             int32_t srcLength)
{
  if(!isWritable()) {
    return *this;
  }

  int32_t oldLength = this->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  if((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    if(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      return *this;
    } else {
      pinIndex(start);
      if(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        return *this;
      }
    }
  }

  if(start == oldLength) {
    return doAppend(srcChars, srcStart, srcLength);
  }

  if(srcChars == 0) {
    srcLength = 0;
  } else {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    if (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  if(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    return *this;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  const UChar *oldArray = getArrayStart();
  if (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    if (copy.isBogus()) {
      setToBogus();
      return *this;
    }
    return doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=FALSE) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  UChar oldStackBuffer[US_STACKBUF_SIZE];
  if((fUnion.fFields.fLengthAndFlags&kUsingStackBuffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(oldStackBuffer, oldArray, oldLength);
    oldArray = oldStackBuffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = 0;
  if(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         FALSE, &bufferToDelete)
  ) {
    return *this;
  }

  // now do the replace

  UChar *newArray = getArrayStart();
  if(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } else if(length != srcLength) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, start, srcLength);

  setLength(newLength);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  if (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  return *this;
}