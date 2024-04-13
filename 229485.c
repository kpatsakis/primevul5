UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                         int32_t desiredCapacityHint,
                                         UChar *scratch, int32_t scratchCapacity,
                                         int32_t *resultCapacity) {
  if(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    return NULL;
  }
  int32_t oldLength = str.length();
  if(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    return str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  return scratch;
}