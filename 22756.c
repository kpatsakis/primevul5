code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if (ONIGENC_IS_CODE_ASCII(code)) return 1;
  else if (code > 0xffffff) return 0;
  else if ((code & 0xff0000) >= 0x800000) return 3;
  else if ((code &   0xff00) >= 0x8000) return 2;
  else
    return ONIGERR_INVALID_CODE_POINT_VALUE;
}