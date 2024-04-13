UnicodeString::indexOf(const UChar *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) const
{
  if(isBogus() || srcChars == 0 || srcStart < 0 || srcLength == 0) {
    return -1;
  }

  // UnicodeString does not find empty substrings
  if(srcLength < 0 && srcChars[srcStart] == 0) {
    return -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  const UChar *array = getArrayStart();
  const UChar *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  if(match == NULL) {
    return -1;
  } else {
    return (int32_t)(match - array);
  }
}