archive_string_default_conversion_for_read(struct archive *a)
{
	const char *cur_charset = get_current_charset(a);
	char oemcp[16];

	/* NOTE: a check of cur_charset is unneeded but we need
	 * that get_current_charset() has been surely called at
	 * this time whatever C compiler optimized. */
	if (cur_charset != NULL &&
	    (a->current_codepage == CP_C_LOCALE ||
	     a->current_codepage == a->current_oemcp))
		return (NULL);/* no conversion. */

	_snprintf(oemcp, sizeof(oemcp)-1, "CP%d", a->current_oemcp);
	/* Make sure a null termination must be set. */
	oemcp[sizeof(oemcp)-1] = '\0';
	return (get_sconv_object(a, oemcp, cur_charset,
	    SCONV_FROM_CHARSET));
}