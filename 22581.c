big5_uao_mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc ARG_UNUSED)
{
    return big5_mbc_enc_len0(p, e, 2, EncLen_BIG5_UAO);
}