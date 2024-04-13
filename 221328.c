rb_str_is_ascii_only_p(VALUE str)
{
    int cr = rb_enc_str_coderange(str);

    return cr == ENC_CODERANGE_7BIT ? Qtrue : Qfalse;
}