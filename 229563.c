UnicodeString::padLeading(int32_t targetLength,
                          UChar padChar)
{
  int32_t oldLength = length();
  if(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    return FALSE;
  } else {
    // move contents up by padding width
    UChar *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    while(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    return TRUE;
  }
}