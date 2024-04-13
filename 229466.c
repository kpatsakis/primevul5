UnicodeString::extract(int32_t start, int32_t len,
                       char *target, uint32_t dstSize) const {
  // if the arguments are illegal, then do nothing
  if(/*dstSize < 0 || */(dstSize > 0 && target == 0)) {
    return 0;
  }
  return toUTF8(start, len, target, dstSize <= 0x7fffffff ? (int32_t)dstSize : 0x7fffffff);
}