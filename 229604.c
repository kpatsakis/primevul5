UnicodeString &UnicodeString::operator=(UnicodeString &&src) U_NOEXCEPT {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, TRUE);
  return *this;
}