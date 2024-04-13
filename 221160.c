rb_str_buf_cat_ascii(VALUE str, const char *ptr)
{
    /* ptr must reference NUL terminated ASCII string. */
    int encindex = ENCODING_GET(str);
    rb_encoding *enc = rb_enc_from_index(encindex);
    if (rb_enc_asciicompat(enc)) {
        return rb_enc_cr_str_buf_cat(str, ptr, strlen(ptr),
            encindex, ENC_CODERANGE_7BIT, 0);
    }
    else {
        char *buf = ALLOCA_N(char, rb_enc_mbmaxlen(enc));
        while (*ptr) {
            unsigned int c = (unsigned char)*ptr;
            int len = rb_enc_codelen(c, enc);
            rb_enc_mbcput(c, buf, enc);
            rb_enc_cr_str_buf_cat(str, buf, len,
                encindex, ENC_CODERANGE_VALID, 0);
            ptr++;
        }
        return str;
    }
}