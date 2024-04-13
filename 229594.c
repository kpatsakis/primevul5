UnicodeStringAppendable::appendString(const UChar *s, int32_t length) {
  return str.doAppend(s, 0, length).isWritable();
}