archive_mstring_clean(struct archive_mstring *aes)
{
	archive_wstring_free(&(aes->aes_wcs));
	archive_string_free(&(aes->aes_mbs));
	archive_string_free(&(aes->aes_utf8));
	archive_string_free(&(aes->aes_mbs_in_locale));
	aes->aes_set = 0;
}