UnicodeString::doLastIndexOf(UChar c,
                 int32_t start,
                 int32_t length) const
{
  if(isBogus()) {
    return -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  const UChar *array = getArrayStart();
  const UChar *match = u_memrchr(array + start, c, length);
  if(match == NULL) {
    return -1;
  } else {
    return (int32_t)(match - array);
  }
}