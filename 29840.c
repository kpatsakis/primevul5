static bool push_ascii_string(void *dest, const char *src, size_t dest_len, int flags, size_t *converted_size)
{
	size_t src_len;
	bool ret;

	if (flags & STR_UPPER) {
		char *tmpbuf = strupper_talloc(NULL, src);
		if (tmpbuf == NULL) {
			return false;
		}
		ret = push_ascii_string(dest, tmpbuf, dest_len, flags & ~STR_UPPER, converted_size);
		talloc_free(tmpbuf);
		return ret;
	}

	src_len = strlen(src);

	if (flags & (STR_TERMINATE | STR_TERMINATE_ASCII))
		src_len++;

	return convert_string(CH_UNIX, CH_DOS, src, src_len, dest, dest_len, converted_size);
}
