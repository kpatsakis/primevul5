rb_str_valid_encoding_p(VALUE str)
{
    int cr = rb_enc_str_coderange(str);

    return cr == ENC_CODERANGE_BROKEN ? Qfalse : Qtrue;
}