rb_external_str_new_with_enc(const char *ptr, long len, rb_encoding *eenc)
{
    VALUE str;

    str = rb_tainted_str_new(ptr, len);
    if (eenc == rb_usascii_encoding() &&
	rb_enc_str_coderange(str) != ENC_CODERANGE_7BIT) {
	rb_enc_associate(str, rb_ascii8bit_encoding());
	return str;
    }
    rb_enc_associate(str, eenc);
    return rb_str_conv_enc(str, eenc, rb_default_internal_encoding());
}