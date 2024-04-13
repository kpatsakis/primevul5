euctw_mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  return onigenc_mbn_mbc_to_code(enc, p, end);
}