sym_printable(const char *s, const char *send, rb_encoding *enc)
{
    while (s < send) {
	int n;
	int c = rb_enc_codepoint_len(s, send, &n, enc);

	if (!rb_enc_isprint(c, enc)) return FALSE;
	s += n;
    }
    return TRUE;
}