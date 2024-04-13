rb_str_each_char(VALUE str)
{
    VALUE orig = str;
    long i, len, n;
    const char *ptr;
    rb_encoding *enc;

    RETURN_ENUMERATOR(str, 0, 0);
    str = rb_str_new4(str);
    ptr = RSTRING_PTR(str);
    len = RSTRING_LEN(str);
    enc = rb_enc_get(str);
    switch (ENC_CODERANGE(str)) {
      case ENC_CODERANGE_VALID:
      case ENC_CODERANGE_7BIT:
	for (i = 0; i < len; i += n) {
	    n = rb_enc_fast_mbclen(ptr + i, ptr + len, enc);
	    rb_yield(rb_str_subseq(str, i, n));
	}
	break;
      default:
	for (i = 0; i < len; i += n) {
	    n = rb_enc_mbclen(ptr + i, ptr + len, enc);
	    rb_yield(rb_str_subseq(str, i, n));
	}
    }
    return orig;
}