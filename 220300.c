best_effort_strncat_from_utf16be(struct archive_string *as, const void *_p,
    size_t bytes, struct archive_string_conv *sc)
{
	return (best_effort_strncat_from_utf16(as, _p, bytes, sc, 1));
}