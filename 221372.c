rb_enc_str_coderange(VALUE str)
{
    int cr = ENC_CODERANGE(str);

    if (cr == ENC_CODERANGE_UNKNOWN) {
	rb_encoding *enc = STR_ENC_GET(str);
        cr = coderange_scan(RSTRING_PTR(str), RSTRING_LEN(str), enc);
        ENC_CODERANGE_SET(str, cr);
    }
    return cr;
}