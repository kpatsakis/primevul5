int http_parse_querystring(const char *txt_, void (*fn)(const char *name, const char *value))
{
	char *o, *t, *txt, *value = NULL, c;

	if (!txt_)
		return 0;

	o = t = txt = xstrdup(txt_);
	while ((c=*t) != '\0') {
		if (c == '=') {
			*t = '\0';
			value = t + 1;
		} else if (c == '+') {
			*t = ' ';
		} else if (c == '%') {
			t = convert_query_hexchar(t);
		} else if (c == '&') {
			*t = '\0';
			(*fn)(txt, value);
			txt = t + 1;
			value = NULL;
		}
		t++;
	}
	if (t != txt)
		(*fn)(txt, value);
	free(o);
	return 0;
}