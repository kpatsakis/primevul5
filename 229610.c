UnicodeString::moveIndex32(int32_t index, int32_t delta) const {
  // pin index
  int32_t len = length();
  if(index<0) {
    index=0;
  } else if(index>len) {
    index=len;
  }

  const UChar *array = getArrayStart();
  if(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } else {
    U16_BACK_N(array, 0, index, -delta);
  }

  return index;
}