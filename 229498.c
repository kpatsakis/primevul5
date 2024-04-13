UnicodeString::doEquals(const UnicodeString &text, int32_t len) const {
  // Requires: this & text not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  return uprv_memcmp(getArrayStart(), text.getArrayStart(), len * U_SIZEOF_UCHAR) == 0;
}