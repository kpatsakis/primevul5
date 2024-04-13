make_codepage_from_charset(const char *charset)
{
	char cs[16];
	char *p;
	unsigned cp;
	int a, b;

	if (charset == NULL || strlen(charset) > 15)
		return -1;

	/* Copy name to uppercase. */
	p = cs;
	while (*charset) {
		char c = *charset++;
		if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';
		*p++ = c;
	}
	*p++ = '\0';
	cp = -1;

	/* Look it up in the table first, so that we can easily
	 * override CP367, which we map to 1252 instead of 367. */
	a = 0;
	b = sizeof(charsets)/sizeof(charsets[0]);
	while (b > a) {
		int c = (b + a) / 2;
		int r = strcmp(charsets[c].name, cs);
		if (r < 0)
			a = c + 1;
		else if (r > 0)
			b = c;
		else
			return charsets[c].cp;
	}

	/* If it's not in the table, try to parse it. */
	switch (*cs) {
	case 'C':
		if (cs[1] == 'P' && cs[2] >= '0' && cs[2] <= '9') {
			cp = my_atoi(cs + 2);
		} else if (strcmp(cs, "CP_ACP") == 0)
			cp = get_current_codepage();
		else if (strcmp(cs, "CP_OEMCP") == 0)
			cp = get_current_oemcp();
		break;
	case 'I':
		if (cs[1] == 'B' && cs[2] == 'M' &&
		    cs[3] >= '0' && cs[3] <= '9') {
			cp = my_atoi(cs + 3);
		}
		break;
	case 'W':
		if (strncmp(cs, "WINDOWS-", 8) == 0) {
			cp = my_atoi(cs + 8);
			if (cp != 874 && (cp < 1250 || cp > 1258))
				cp = -1;/* This may invalid code. */
		}
		break;
	}
	return (cp);
}