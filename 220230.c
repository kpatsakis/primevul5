archive_strncpy_l(struct archive_string *as, const void *_p, size_t n,
    struct archive_string_conv *sc)
{
	as->length = 0;
	return (archive_strncat_l(as, _p, n, sc));
}