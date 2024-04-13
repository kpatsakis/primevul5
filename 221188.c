rb_str_splice(VALUE str, long beg, long len, VALUE val)
{
    long slen;
    char *p, *e;
    rb_encoding *enc;
    int singlebyte = single_byte_optimizable(str);
    int cr;

    if (len < 0) rb_raise(rb_eIndexError, "negative length %ld", len);

    StringValue(val);
    enc = rb_enc_check(str, val);
    slen = str_strlen(str, enc);

    if (slen < beg) {
      out_of_range:
	rb_raise(rb_eIndexError, "index %ld out of string", beg);
    }
    if (beg < 0) {
	if (-beg > slen) {
	    goto out_of_range;
	}
	beg += slen;
    }
    if (slen < len || slen < beg + len) {
	len = slen - beg;
    }
    str_modify_keep_cr(str);
    p = str_nth(RSTRING_PTR(str), RSTRING_END(str), beg, enc, singlebyte);
    if (!p) p = RSTRING_END(str);
    e = str_nth(p, RSTRING_END(str), len, enc, singlebyte);
    if (!e) e = RSTRING_END(str);
    /* error check */
    beg = p - RSTRING_PTR(str);	/* physical position */
    len = e - p;		/* physical length */
    rb_str_splice_0(str, beg, len, val);
    rb_enc_associate(str, enc);
    cr = ENC_CODERANGE_AND(ENC_CODERANGE(str), ENC_CODERANGE(val));
    if (cr != ENC_CODERANGE_BROKEN)
	ENC_CODERANGE_SET(str, cr);
}