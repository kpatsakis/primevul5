insert_value(char **str, char *pos, const char *substr, char **next)
{
	size_t subsz = strlen(substr);			/* substring size */
	size_t strsz = strlen(*str);
	size_t possz = strlen(pos);
	size_t posoff;
	char *p;
	int sep;

	/* is it necessary to prepend '=' before the substring ? */
	sep = !(pos > *str && *(pos - 1) == '=');

	/* save an offset of the place where we need to add substr */
	posoff = pos - *str;

	p = realloc(*str, strsz + sep + subsz + 1);
	if (!p)
		return -ENOMEM;

	/* zeroize the newly allocated memory -- valgrind loves us... */
	memset(p + strsz, 0, sep + subsz + 1);

	/* set pointers to the reallocated string */
	*str = p;
	pos = p + posoff;

	if (possz)
		/* create a room for the new substring */
		memmove(pos + subsz + sep, pos, possz + 1);
	if (sep)
		*pos++ = '=';

	memcpy(pos, substr, subsz);

	if (next) {
		/* set pointer to the next option */
		*next = pos + subsz;
		if (**next == ',')
			(*next)++;
	}
	return 0;
}