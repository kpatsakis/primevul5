_PUBLIC_ void string_replace_m(char *s, char oldc, char newc)
{
	struct smb_iconv_handle *ic = get_iconv_handle();
	while (s && *s) {
		size_t size;
		codepoint_t c = next_codepoint_handle(ic, s, &size);
		if (c == oldc) {
			*s = newc;
		}
		s += size;
	}
}
