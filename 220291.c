archive_mstring_copy_mbs_len(struct archive_mstring *aes, const char *mbs,
    size_t len)
{
	if (mbs == NULL) {
		aes->aes_set = 0;
		return (0);
	}
	aes->aes_set = AES_SET_MBS; /* Only MBS form is set now. */
	archive_strncpy(&(aes->aes_mbs), mbs, len);
	archive_string_empty(&(aes->aes_utf8));
	archive_wstring_empty(&(aes->aes_wcs));
	return (0);
}