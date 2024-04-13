rb_str_conv_enc_opts(VALUE str, rb_encoding *from, rb_encoding *to, int ecflags, VALUE ecopts)
{
    rb_econv_t *ec;
    rb_econv_result_t ret;
    long len;
    VALUE newstr;
    const unsigned char *sp;
    unsigned char *dp;

    if (!to) return str;
    if (from == to) return str;
    if ((rb_enc_asciicompat(to) && ENC_CODERANGE(str) == ENC_CODERANGE_7BIT) ||
	to == rb_ascii8bit_encoding()) {
	if (STR_ENC_GET(str) != to) {
	    str = rb_str_dup(str);
	    rb_enc_associate(str, to);
	}
	return str;
    }

    len = RSTRING_LEN(str);
    newstr = rb_str_new(0, len);

  retry:
    ec = rb_econv_open_opts(from->name, to->name, ecflags, ecopts);
    if (!ec) return str;

    sp = (unsigned char*)RSTRING_PTR(str);
    dp = (unsigned char*)RSTRING_PTR(newstr);
    ret = rb_econv_convert(ec, &sp, (unsigned char*)RSTRING_END(str),
			   &dp, (unsigned char*)RSTRING_END(newstr), 0);
    rb_econv_close(ec);
    switch (ret) {
      case econv_destination_buffer_full:
	/* destination buffer short */
	len *= 2;
	rb_str_resize(newstr, len);
	goto retry;

      case econv_finished:
	len = dp - (unsigned char*)RSTRING_PTR(newstr);
	rb_str_set_len(newstr, len);
	rb_enc_associate(newstr, to);
	return newstr;

      default:
	/* some error, return original */
	return str;
    }
}