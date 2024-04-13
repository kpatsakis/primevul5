UnicodeString::doReverse(int32_t start, int32_t length) {
  if(length <= 1 || !cloneArrayIfNeeded()) {
    return *this;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  if(length <= 1) {  // pinIndices() might have shrunk the length
    return *this;
  }

  UChar *left = getArrayStart() + start;
  UChar *right = left + length - 1;  // -1 for inclusive boundary (length>=2)
  UChar swap;
  UBool hasSupplementary = FALSE;

  // Before the loop we know left<right because length>=2.
  do {
    hasSupplementary |= (UBool)U16_IS_LEAD(swap = *left);
    hasSupplementary |= (UBool)U16_IS_LEAD(*left++ = *right);
    *right-- = swap;
  } while(left < right);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= (UBool)U16_IS_LEAD(*left);

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  if(hasSupplementary) {
    UChar swap2;

    left = getArrayStart() + start;
    right = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    while(left < right) {
      if(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } else {
        ++left;
      }
    }
  }

  return *this;
}