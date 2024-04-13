static ssize_t push_ucs2(void *dest, const char *src, size_t dest_len, int flags)
{
	size_t len=0;
	size_t src_len = strlen(src);
	size_t size = 0;
	bool ret;

	if (flags & STR_UPPER) {
		char *tmpbuf = strupper_talloc(NULL, src);
		ssize_t retval;
		if (tmpbuf == NULL) {
			return -1;
		}
		retval = push_ucs2(dest, tmpbuf, dest_len, flags & ~STR_UPPER);
		talloc_free(tmpbuf);
		return retval;
	}

	if (flags & STR_TERMINATE)
		src_len++;

	if (ucs2_align(NULL, dest, flags)) {
		*(char *)dest = 0;
		dest = (void *)((char *)dest + 1);
		if (dest_len) dest_len--;
		len++;
	}

	/* ucs2 is always a multiple of 2 bytes */
	dest_len &= ~1;

	ret = convert_string(CH_UNIX, CH_UTF16, src, src_len, dest, dest_len, &size);
	if (ret == false) {
		return 0;
	}

	len += size;

	return (ssize_t)len;
}
