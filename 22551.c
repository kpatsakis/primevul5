us_ascii_mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc)
{
    if (*p & 0x80)
        return ONIGENC_CONSTRUCT_MBCLEN_INVALID();
    return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(1);
}