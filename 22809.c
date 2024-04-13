euctw_code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  return onigenc_mb4_code_to_mbc(enc, code, buf);
}