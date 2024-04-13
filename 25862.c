static char *clean_path(char *path)
{
	char *ch;
	char *ch2;
	char *str;
	str = xmalloc(strlen(path) + 1);
	ch = path;
	ch2 = str;
	while (true) {
		*ch2 = *ch;
		ch++;
		ch2++;
		if (!*(ch-1))
			break;
		while (*(ch - 1) == '/' && *ch == '/')
			ch++;
	}
	/* get rid of trailing / characters */
	while ((ch = strrchr(str, '/'))) {
		if (ch == str)
			break;
		if (!*(ch+1))
			*ch = 0;
		else
			break;
	}
	return str;
}