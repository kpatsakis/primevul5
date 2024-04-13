vte_ucs4_to_utf8 (VteTerminal *terminal, const guchar *in)
{
	gchar *out = NULL;
	guchar *buf = NULL, *bufptr = NULL;
	gsize inlen, outlen;
	VteConv conv;

	conv = _vte_conv_open ("UTF-8", VTE_CONV_GUNICHAR_TYPE);

	if (conv != VTE_INVALID_CONV) {
		inlen = vte_unichar_strlen ((gunichar *) in) * sizeof (gunichar);
		outlen = (inlen * VTE_UTF8_BPC) + 1;

		_vte_buffer_set_minimum_size (terminal->pvt->conv_buffer, outlen);
		buf = bufptr = terminal->pvt->conv_buffer->data;

		if (_vte_conv (conv, &in, &inlen, &buf, &outlen) == (size_t) -1) {
			_vte_debug_print (VTE_DEBUG_IO,
					  "Error converting %ld string bytes (%s), skipping.\n",
					  (long) _vte_buffer_length (terminal->pvt->outgoing),
					  g_strerror (errno));
			bufptr = NULL;
		} else {
			out = g_strndup ((gchar *) bufptr, buf - bufptr);
		}
	}

	_vte_conv_close (conv);

	return out;
}