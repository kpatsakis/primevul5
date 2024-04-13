archive_string_concat(struct archive_string *dest, struct archive_string *src)
{
	if (archive_string_append(dest, src->s, src->length) == NULL)
		__archive_errx(1, "Out of memory");
}