UnicodeString::UnicodeString(const UChar *text) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, -1);
}