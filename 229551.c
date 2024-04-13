UnicodeString::doIndexOf(UChar c,
             int32_t start,
             int32_t length) const
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  const UChar *array = getArrayStart();
  const UChar *match = u_memchr(array + start, c, length);
  if(match == NULL) {
    return -1;
  } else {
    return (int32_t)(match - array);
  }
}