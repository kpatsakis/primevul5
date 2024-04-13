euctw_is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc)
{
  return onigenc_mb4_is_code_ctype(enc, code, ctype);
}