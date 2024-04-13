UnicodeString::UnicodeString(UnicodeString &&src) U_NOEXCEPT {
  copyFieldsFrom(src, TRUE);
}