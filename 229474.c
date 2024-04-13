UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  const UChar *text = textPtr;
  if(text == NULL) {
    // treat as an empty string, do not alias
    setToEmpty();
  } else if(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } else {
    if(textLength == -1) {
      // text is terminated, or else it would have failed the above test
      textLength = u_strlen(text);
    }
    setArray(const_cast<UChar *>(text), textLength,
             isTerminated ? textLength + 1 : textLength);
  }
}