get_current_charset(struct archive *a)
{
	const char *cur_charset;

	if (a == NULL)
		cur_charset = default_iconv_charset("");
	else {
		cur_charset = default_iconv_charset(a->current_code);
		if (a->current_code == NULL) {
			a->current_code = strdup(cur_charset);
			a->current_codepage = get_current_codepage();
			a->current_oemcp = get_current_oemcp();
		}
	}
	return (cur_charset);
}