static size_t pull_ucs2(char *dest, const void *src, size_t dest_len, size_t src_len, int flags)
{
	size_t size = 0;

	if (ucs2_align(NULL, src, flags)) {
		src = (const void *)((const char *)src + 1);
		if (src_len > 0)
			src_len--;
	}

	if (flags & STR_TERMINATE) {
		if (src_len == (size_t)-1) {
			src_len = utf16_len(src);
		} else {
			src_len = utf16_len_n(src, src_len);
		}
	}

	/* ucs2 is always a multiple of 2 bytes */
	if (src_len != (size_t)-1)
		src_len &= ~1;

	/* We're ignoring the return here.. */
	(void)convert_string(CH_UTF16, CH_UNIX, src, src_len, dest, dest_len, &size);
	if (dest_len)
		dest[MIN(size, dest_len-1)] = 0;

	return src_len;
}
