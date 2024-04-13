UnicodeString::unBogus() {
  if(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}