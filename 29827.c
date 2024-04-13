_PUBLIC_ ssize_t push_codepoint(char *str, codepoint_t c)
{
	return push_codepoint_handle(get_iconv_handle(), str, c);
}
