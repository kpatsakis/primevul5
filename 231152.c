ldns_strip_ws(char *line)
{
	char *s = line, *e;

	for (s = line; *s && isspace((unsigned char)*s); s++)
		;

	for (e = strchr(s, 0); e > s+2 && isspace((unsigned char)e[-1]) && e[-2] != '\\'; e--)
		;
	*e = 0;

	return s;
}