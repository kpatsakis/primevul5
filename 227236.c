char *bcf_hdr_fmt_text(const bcf_hdr_t *hdr, int is_bcf, int *len)
{
    kstring_t txt = {0,0,0};
    bcf_hdr_format(hdr, is_bcf, &txt);
    if ( len ) *len = txt.l;
    return txt.s;
}