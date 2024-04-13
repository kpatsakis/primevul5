UnicodeStringAppendable::appendCodeUnit(UChar c) {
  return str.doAppend(&c, 0, 1).isWritable();
}