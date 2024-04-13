UnicodeString::releaseArray() {
  if((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free((int32_t *)fUnion.fFields.fArray - 1);
  }
}