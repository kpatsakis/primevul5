get_current_oemcp(void)
{
	int i;
	char *locale, *p;
	size_t len;

	locale = setlocale(LC_CTYPE, NULL);
	if (locale == NULL)
		return (GetOEMCP());
	if (locale[0] == 'C' && locale[1] == '\0')
		return (CP_C_LOCALE);

	p = strrchr(locale, '.');
	if (p == NULL)
		return (GetOEMCP());
	len = p - locale;
	for (i = 0; acp_ocp_map[i].acp; i++) {
		if (strncmp(acp_ocp_map[i].locale, locale, len) == 0)
			return (acp_ocp_map[i].ocp);
	}
	return (GetOEMCP());
}