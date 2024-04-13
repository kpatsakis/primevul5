void html_url_arg(const char *txt)
{
	const char *t = txt;
	while (t && *t) {
		unsigned char c = *t;
		const char *e = url_escape_table[c];
		if (c == ' ')
			e = "+";
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