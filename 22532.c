code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if (code < 256) {
    if (EncLen_SJIS[(int )code] == 1)
      return 1;
    else
      return 0;
  }
  else if (code <= 0xffff) {
    return 2;
  }
  else
    return ONIGERR_INVALID_CODE_POINT_VALUE;
}