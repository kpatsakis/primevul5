gbk_is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc)
{
  return onigenc_mb2_is_code_ctype(enc, code, ctype);
}