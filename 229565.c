UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  return str.cloneArrayIfNeeded(str.length() + appendCapacity);
}