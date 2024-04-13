UnicodeString::UnicodeString(const char *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  if(src==NULL) {
    // treat as an empty string
  } else {
    if(length<0) {
      length=(int32_t)uprv_strlen(src);
    }
    if(cloneArrayIfNeeded(length, length, FALSE)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } else {
      setToBogus();
    }
  }
}