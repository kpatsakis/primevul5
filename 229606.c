UnicodeString::setCharAt(int32_t offset,
             UChar c)
{
  int32_t len = length();
  if(cloneArrayIfNeeded() && len > 0) {
    if(offset < 0) {
      offset = 0;
    } else if(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  return *this;
}