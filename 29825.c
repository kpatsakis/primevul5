_PUBLIC_ codepoint_t next_codepoint_ext(const char *str, size_t len,
					charset_t src_charset, size_t *size)
{
	return next_codepoint_handle_ext(get_iconv_handle(), str, len,
					 src_charset, size);
}
