archive_wstrappend_wchar(struct archive_wstring *as, wchar_t c)
{
	if ((as = archive_wstring_append(as, &c, 1)) == NULL)
		__archive_errx(1, "Out of memory");
	return (as);
}