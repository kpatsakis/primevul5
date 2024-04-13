UnicodeString::doExtract(int32_t start,
             int32_t length,
             UChar *dst,
             int32_t dstStart) const
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  const UChar *array = getArrayStart();
  if(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}