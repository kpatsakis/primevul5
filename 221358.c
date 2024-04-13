rb_str_concat(VALUE str1, VALUE str2)
{
    if (FIXNUM_P(str2) || TYPE(str2) == T_BIGNUM) {
	rb_encoding *enc = STR_ENC_GET(str1);
	unsigned int c = NUM2UINT(str2);
	long pos = RSTRING_LEN(str1);
	int len = rb_enc_codelen(c, enc);
	int cr = ENC_CODERANGE(str1);

	rb_str_resize(str1, pos+len);
	rb_enc_mbcput(c, RSTRING_PTR(str1)+pos, enc);
	ENC_CODERANGE_SET(str1, cr);
	return str1;
    }
    return rb_str_append(str1, str2);
}