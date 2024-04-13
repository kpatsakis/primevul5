coderange_scan(const char *p, long len, rb_encoding *enc)
{
    const char *e = p + len;

    if (rb_enc_to_index(enc) == 0) {
        /* enc is ASCII-8BIT.  ASCII-8BIT string never be broken. */
        p = search_nonascii(p, e);
        return p ? ENC_CODERANGE_VALID : ENC_CODERANGE_7BIT;
    }

    if (rb_enc_asciicompat(enc)) {
        p = search_nonascii(p, e);
        if (!p) {
            return ENC_CODERANGE_7BIT;
        }
        while (p < e) {
            int ret = rb_enc_precise_mbclen(p, e, enc);
            if (!MBCLEN_CHARFOUND_P(ret)) {
                return ENC_CODERANGE_BROKEN;
            }
            p += MBCLEN_CHARFOUND_LEN(ret);
            if (p < e) {
                p = search_nonascii(p, e);
                if (!p) {
                    return ENC_CODERANGE_VALID;
                }
            }
        }
        if (e < p) {
            return ENC_CODERANGE_BROKEN;
        }
        return ENC_CODERANGE_VALID;
    }

    while (p < e) {
        int ret = rb_enc_precise_mbclen(p, e, enc);

        if (!MBCLEN_CHARFOUND_P(ret)) {
            return ENC_CODERANGE_BROKEN;
        }
        p += MBCLEN_CHARFOUND_LEN(ret);
    }
    if (e < p) {
        return ENC_CODERANGE_BROKEN;
    }
    return ENC_CODERANGE_VALID;
}