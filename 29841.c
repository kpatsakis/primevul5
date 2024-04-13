_PUBLIC_ ssize_t push_string(void *dest, const char *src, size_t dest_len, int flags)
{
	if (flags & STR_ASCII) {
		size_t size = 0;
		if (push_ascii_string(dest, src, dest_len, flags, &size)) {
			return (ssize_t)size;
		} else {
			return (ssize_t)-1;
		}
	} else if (flags & STR_UNICODE) {
		return push_ucs2(dest, src, dest_len, flags);
	} else {
		smb_panic("push_string requires either STR_ASCII or STR_UNICODE flag to be set");
		return -1;
	}
}
