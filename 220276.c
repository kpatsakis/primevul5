archive_mstring_copy_wcs(struct archive_mstring *aes, const wchar_t *wcs)
{
	return archive_mstring_copy_wcs_len(aes, wcs,
				wcs == NULL ? 0 : wcslen(wcs));
}