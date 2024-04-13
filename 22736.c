utf16be_mbc_enc_len(const UChar* p, const OnigUChar* e ARG_UNUSED,
		    OnigEncoding enc ARG_UNUSED)
{
  int byte = p[0];
  if (!UTF16_IS_SURROGATE(byte)) {
    if (2 <= e-p)
      return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(2);
    else
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
  }
  if (UTF16_IS_SURROGATE_FIRST(byte)) {
    switch (e-p) {
      case 1: return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(3);
      case 2: return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(2);
      case 3:
        if (UTF16_IS_SURROGATE_SECOND(p[2]))
          return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
        break;
      default:
        if (UTF16_IS_SURROGATE_SECOND(p[2]))
          return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(4);
        break;
    }
  }
  return ONIGENC_CONSTRUCT_MBCLEN_INVALID();
}