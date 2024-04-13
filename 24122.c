static inline const char *_chr_sanitize(unsigned char c)
{
	if (c >= 0x20 && c < 0x7F)
		return t_strdup_printf("`%c'", c);
	if (c == 0x0a)
		return "<LF>";
	if (c == 0x0d)
		return "<CR>";
	return t_strdup_printf("<0x%02x>", c);
}