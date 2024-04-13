archive_strappend_char(struct archive_string *as, char c)
{
	if ((as = archive_string_append(as, &c, 1)) == NULL)
		__archive_errx(1, "Out of memory");
	return (as);
}