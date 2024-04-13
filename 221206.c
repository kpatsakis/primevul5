rb_str_check_dummy_enc(rb_encoding *enc)
{
    if (rb_enc_dummy_p(enc)) {
	rb_raise(rb_eEncCompatError, "incompatible encoding with this operation: %s",
		 rb_enc_name(enc));
    }
}