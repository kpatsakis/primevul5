rb_str_hash(VALUE str)
{
    int e = ENCODING_GET(str);
    if (e && rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT) {
	e = 0;
    }
    return rb_memhash((const void *)RSTRING_PTR(str), RSTRING_LEN(str)) ^ e;
}