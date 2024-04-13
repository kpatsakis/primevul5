int http_parse_querystring(char *txt, void (*fn)(const char *name, const char *value))
{
	char *t, *value = NULL, c;

	if (!txt)
		return 0;

	t = txt = strdup(txt);
	if (t == NULL) {
		printf("Out of memory\n");
		exit(1);
	}
	while((c=*t) != '\0') {
		if (c=='=') {
			*t = '\0';
			value = t+1;
		} else if (c=='+') {
			*t = ' ';
		} else if (c=='%') {
			t = convert_query_hexchar(t);
		} else if (c=='&') {
			*t = '\0';
			(*fn)(txt, value);
			txt = t+1;
			value = NULL;
		}
		t++;
	}
	if (t!=txt)
		(*fn)(txt, value);
	return 0;
}