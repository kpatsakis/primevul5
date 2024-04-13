UnicodeString::padTrailing(int32_t targetLength,
                           UChar padChar)
{
  int32_t oldLength = length();
  if(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    return FALSE;
  } else {
    // fill in padding character
    UChar *array = getArrayStart();
    int32_t length = targetLength;
    while(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    return TRUE;
  }
}