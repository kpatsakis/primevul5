_PUBLIC_ size_t count_chars_m(const char *s, char c)
{
	struct smb_iconv_handle *ic = get_iconv_handle();
	size_t count = 0;

	while (*s) {
		size_t size;
		codepoint_t c2 = next_codepoint_handle(ic, s, &size);
		if (c2 == c) count++;
		s += size;
	}

	return count;
}
