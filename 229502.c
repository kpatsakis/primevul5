UnicodeString &UnicodeString::setToUTF8(StringPiece utf8) {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  if(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } else {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  UChar *utf16 = getBuffer(capacity);
  int32_t length16;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, getCapacity(), &length16,
      utf8.data(), length,
      0xfffd,  // Substitution character.
      NULL,    // Don't care about number of substitutions.
      &errorCode);
  releaseBuffer(length16);
  if(U_FAILURE(errorCode)) {
    setToBogus();
  }
  return *this;
}