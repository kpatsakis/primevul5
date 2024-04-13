rb_enc_str_asciionly_p(VALUE str)
{
    rb_encoding *enc = STR_ENC_GET(str);

    if (!rb_enc_asciicompat(enc))
        return FALSE;
    else if (rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT)
        return TRUE;
    return FALSE;
}