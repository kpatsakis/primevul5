_PUBLIC_ codepoint_t next_codepoint_handle(struct smb_iconv_handle *ic,
					   const char *str, size_t *size)
{
	/*
	 * We assume that no multi-byte character can take more than 5 bytes
	 * thus avoiding walking all the way down a long string. This is OK as
	 * Unicode codepoints only go up to (U+10ffff), which can always be
	 * encoded in 4 bytes or less.
	 */
	return next_codepoint_handle_ext(ic, str, strnlen(str, 5), CH_UNIX,
					 size);
}
