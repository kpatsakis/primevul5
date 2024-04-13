void html_header_arg_in_quotes(const char *txt)
{
	const char *t = txt;
	while (t && *t) {
		unsigned char c = *t;
		const char *e = NULL;
		if (c == '\\')
			e = "\\\\";
		else if (c == '\r')
			e = "\\r";
		else if (c == '\n')
			e = "\\n";
		else if (c == '"')
			e = "\\\"";
		if (e) {
			html_raw(txt, t - txt);
			html(e);
			txt = t + 1;
		}
		t++;
	}
	if (t != txt)
		html(txt);

}