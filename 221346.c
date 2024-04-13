rb_enc_strlen(const char *p, const char *e, rb_encoding *enc)
{
    return enc_strlen(p, e, enc, ENC_CODERANGE_UNKNOWN);
}