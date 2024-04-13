smb_iconv_t get_conv_handle(struct smb_iconv_handle *ic,
			    charset_t from, charset_t to)
{
	const char *n1, *n2;

	if (ic->conv_handles[from][to]) {
		return ic->conv_handles[from][to];
	}

	n1 = charset_name(ic, from);
	n2 = charset_name(ic, to);

	ic->conv_handles[from][to] = smb_iconv_open_ex(ic, n2, n1,
						       ic->use_builtin_handlers);

	if (ic->conv_handles[from][to] == (smb_iconv_t)-1) {
		if ((from == CH_DOS || to == CH_DOS) &&
		    strcasecmp(charset_name(ic, CH_DOS), "ASCII") != 0) {
			DEBUG(0,("dos charset '%s' unavailable - using ASCII\n",
				 charset_name(ic, CH_DOS)));
			ic->dos_charset = "ASCII";

			n1 = charset_name(ic, from);
			n2 = charset_name(ic, to);

			ic->conv_handles[from][to] =
				smb_iconv_open_ex(ic, n2, n1, ic->use_builtin_handlers);
		}
	}

	return ic->conv_handles[from][to];
}
