UnicodeString::UnicodeString(const char *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  if(codepageData != 0) {
    setToUTF8(codepageData);
  }
}