UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       const UChar *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) const
{
  // compare illegal string values
  // treat const UChar *srcChars==NULL as an empty string
  if(isBogus()) {
    return -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  if(srcChars == NULL) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=NULL)?(srcChars + srcStart):NULL, srcLength, FALSE, TRUE);
  /* translate the 32-bit result into an 8-bit one */
  if(diff!=0) {
    return (int8_t)(diff >> 15 | 1);
  } else {
    return 0;
  }
}