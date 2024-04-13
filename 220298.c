best_effort_strncat_to_utf16be(struct archive_string *as16, const void *_p,
    size_t length, struct archive_string_conv *sc)
{
	return (best_effort_strncat_to_utf16(as16, _p, length, sc, 1));
}