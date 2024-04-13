_PUBLIC_ ssize_t pull_string(char *dest, const void *src, size_t dest_len, size_t src_len, int flags)
{
	if (flags & STR_ASCII) {
		return pull_ascii_string(dest, src, dest_len, src_len, flags);
	} else if (flags & STR_UNICODE) {
		return pull_ucs2(dest, src, dest_len, src_len, flags);
	} else {
		smb_panic("pull_string requires either STR_ASCII or STR_UNICODE flag to be set");
		return -1;
	}
}
