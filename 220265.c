archive_string_conversion_from_charset(struct archive *a, const char *charset,
    int best_effort)
{
	int flag = SCONV_FROM_CHARSET;

	if (best_effort)
		flag |= SCONV_BEST_EFFORT;
	return (get_sconv_object(a, charset, get_current_charset(a), flag));
}