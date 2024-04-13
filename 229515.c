UnicodeString::tempSubString(int32_t start, int32_t len) const {
  pinIndices(start, len);
  const UChar *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  if(array==NULL) {
    array=fUnion.fStackFields.fBuffer;  // anything not NULL because that would make an empty string
    len=-2;  // bogus result string
  }
  return UnicodeString(FALSE, array + start, len);
}