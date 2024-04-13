archive_wstring_concat(struct archive_wstring *dest,
    struct archive_wstring *src)
{
	if (archive_wstring_append(dest, src->s, src->length) == NULL)
		__archive_errx(1, "Out of memory");
}