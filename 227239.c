int bcf_enc_vchar(kstring_t *s, int l, const char *a)
{
    bcf_enc_size(s, l, BCF_BT_CHAR);
    kputsn(a, l, s);
    return 0; // FIXME: check for errs in this function
}