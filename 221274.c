single_byte_optimizable(VALUE str)
{
    rb_encoding *enc;

    /* Conservative.  It may be ENC_CODERANGE_UNKNOWN. */
    if (ENC_CODERANGE(str) == ENC_CODERANGE_7BIT)
        return 1;

    enc = STR_ENC_GET(str);
    if (rb_enc_mbmaxlen(enc) == 1)
        return 1;

    /* Conservative.  Possibly single byte.
     * "\xa1" in Shift_JIS for example. */
    return 0;
}