rb_enc_cr_str_exact_copy(VALUE dest, VALUE src)
{
    str_enc_copy(dest, src);
    ENC_CODERANGE_SET(dest, ENC_CODERANGE(src));
}