parse_comma_separated_list(char **startptr, bool *more)
{
	char	   *p;
	char	   *s = *startptr;
	char	   *e;
	int			len;

	/*
	 * Search for the end of the current element; a comma or end-of-string
	 * acts as a terminator.
	 */
	e = s;
	while (*e != '\0' && *e != ',')
		++e;
	*more = (*e == ',');

	len = e - s;
	p = (char *) malloc(sizeof(char) * (len + 1));
	if (p)
	{
		memcpy(p, s, len);
		p[len] = '\0';
	}
	*startptr = e + 1;

	return p;
}