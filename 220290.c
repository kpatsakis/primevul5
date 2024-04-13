win_strncat_from_utf16le(struct archive_string *as, const void *_p,
    size_t bytes, struct archive_string_conv *sc)
{
	return (win_strncat_from_utf16(as, _p, bytes, sc, 0));
}