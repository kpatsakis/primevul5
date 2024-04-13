rb_str_hex(VALUE str)
{
    rb_encoding *enc = rb_enc_get(str);

    if (!rb_enc_asciicompat(enc)) {
	rb_raise(rb_eEncCompatError, "ASCII incompatible encoding: %s", rb_enc_name(enc));
    }
    return rb_str_to_inum(str, 16, FALSE);
}