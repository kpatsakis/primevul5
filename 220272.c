archive_mstring_copy_mbs(struct archive_mstring *aes, const char *mbs)
{
	if (mbs == NULL) {
		aes->aes_set = 0;
		return (0);
	}
	return (archive_mstring_copy_mbs_len(aes, mbs, strlen(mbs)));
}