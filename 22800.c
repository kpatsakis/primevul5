gbk_code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  return onigenc_mb2_code_to_mbc(enc, code, buf);
}