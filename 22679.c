code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if      ((code & 0xffffff80) == 0) return 1;
  else if ((code & 0xfffff800) == 0) return 2;
  else if ((code & 0xffff0000) == 0) return 3;
  else if ((code & 0xffe00000) == 0) return 4;
  else if ((code & 0xfc000000) == 0) return 5;
  else if ((code & 0x80000000) == 0) return 6;
#ifdef USE_INVALID_CODE_SCHEME
  else if (code == INVALID_CODE_FE) return 1;
  else if (code == INVALID_CODE_FF) return 1;
#endif
  else
    return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
}