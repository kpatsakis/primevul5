archive_string_conversion_to_charset(struct archive *a, const char *charset,
    int best_effort)
{
	int flag = SCONV_TO_CHARSET;

	if (best_effort)
		flag |= SCONV_BEST_EFFORT;
	return (get_sconv_object(a, get_current_charset(a), charset, flag));
}