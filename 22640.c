gbk_mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc)
{
  return onigenc_mbn_mbc_to_code(enc, p, end);
}