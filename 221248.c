rb_str_each_codepoint(VALUE str)
{
    VALUE orig = str;
    long len;
    int n;
    unsigned int c;
    const char *ptr, *end;
    rb_encoding *enc;

    if (single_byte_optimizable(str)) return rb_str_each_byte(str);
    RETURN_ENUMERATOR(str, 0, 0);
    str = rb_str_new4(str);
    ptr = RSTRING_PTR(str);
    len = RSTRING_LEN(str);
    end = RSTRING_END(str);
    enc = STR_ENC_GET(str);
    while (ptr < end) {
	c = rb_enc_codepoint_len(ptr, end, &n, enc);
	rb_yield(UINT2NUM(c));
	ptr += n;
    }
    return orig;
}