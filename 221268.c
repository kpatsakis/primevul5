rb_enc_cr_str_buf_cat(VALUE str, const char *ptr, long len,
    int ptr_encindex, int ptr_cr, int *ptr_cr_ret)
{
    int str_encindex = ENCODING_GET(str);
    int res_encindex;
    int str_cr, res_cr;
    int str_a8 = ENCODING_IS_ASCII8BIT(str);
    int ptr_a8 = ptr_encindex == 0;

    str_cr = ENC_CODERANGE(str);

    if (str_encindex == ptr_encindex) {
        if (str_cr == ENC_CODERANGE_UNKNOWN ||
            (ptr_a8 && str_cr != ENC_CODERANGE_7BIT)) {
            ptr_cr = ENC_CODERANGE_UNKNOWN;
        }
        else if (ptr_cr == ENC_CODERANGE_UNKNOWN) {
            ptr_cr = coderange_scan(ptr, len, rb_enc_from_index(ptr_encindex));
        }
    }
    else {
        rb_encoding *str_enc = rb_enc_from_index(str_encindex);
        rb_encoding *ptr_enc = rb_enc_from_index(ptr_encindex);
        if (!rb_enc_asciicompat(str_enc) || !rb_enc_asciicompat(ptr_enc)) {
            if (len == 0)
                return str;
            if (RSTRING_LEN(str) == 0) {
                rb_str_buf_cat(str, ptr, len);
                ENCODING_CODERANGE_SET(str, ptr_encindex, ptr_cr);
                return str;
            }
            goto incompatible;
        }
	if (ptr_cr == ENC_CODERANGE_UNKNOWN) {
	    ptr_cr = coderange_scan(ptr, len, ptr_enc);
	}
        if (str_cr == ENC_CODERANGE_UNKNOWN) {
            if (str_a8 || ptr_cr != ENC_CODERANGE_7BIT) {
                str_cr = rb_enc_str_coderange(str);
            }
        }
    }
    if (ptr_cr_ret)
        *ptr_cr_ret = ptr_cr;

    if (str_encindex != ptr_encindex &&
        str_cr != ENC_CODERANGE_7BIT &&
        ptr_cr != ENC_CODERANGE_7BIT) {
      incompatible:
        rb_raise(rb_eEncCompatError, "incompatible character encodings: %s and %s",
            rb_enc_name(rb_enc_from_index(str_encindex)),
            rb_enc_name(rb_enc_from_index(ptr_encindex)));
    }

    if (str_cr == ENC_CODERANGE_UNKNOWN) {
        res_encindex = str_encindex;
        res_cr = ENC_CODERANGE_UNKNOWN;
    }
    else if (str_cr == ENC_CODERANGE_7BIT) {
        if (ptr_cr == ENC_CODERANGE_7BIT) {
            res_encindex = !str_a8 ? str_encindex : ptr_encindex;
            res_cr = ENC_CODERANGE_7BIT;
        }
        else {
            res_encindex = ptr_encindex;
            res_cr = ptr_cr;
        }
    }
    else if (str_cr == ENC_CODERANGE_VALID) {
        res_encindex = str_encindex;
        res_cr = str_cr;
    }
    else { /* str_cr == ENC_CODERANGE_BROKEN */
        res_encindex = str_encindex;
        res_cr = str_cr;
        if (0 < len) res_cr = ENC_CODERANGE_UNKNOWN;
    }

    if (len < 0) {
	rb_raise(rb_eArgError, "negative string size (or size too big)");
    }
    str_buf_cat(str, ptr, len);
    ENCODING_CODERANGE_SET(str, res_encindex, res_cr);
    return str;
}