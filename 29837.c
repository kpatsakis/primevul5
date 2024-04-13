static ssize_t pull_ascii_string(char *dest, const void *src, size_t dest_len, size_t src_len, int flags)
{
	size_t size = 0;

	if (flags & (STR_TERMINATE | STR_TERMINATE_ASCII)) {
		if (src_len == (size_t)-1) {
			src_len = strlen((const char *)src) + 1;
		} else {
			size_t len = strnlen((const char *)src, src_len);
			if (len < src_len)
				len++;
			src_len = len;
		}
	}

	/* We're ignoring the return here.. */
	(void)convert_string(CH_DOS, CH_UNIX, src, src_len, dest, dest_len, &size);

	if (dest_len)
		dest[MIN(size, dest_len-1)] = 0;

	return src_len;
}
