onigenc_mb2_code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if ((code & 0xff00) != 0) return 2;
  else return 1;
}