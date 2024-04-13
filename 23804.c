void html_url_path(const char *txt)
{
	const char *t = txt;
	while (t && *t) {
		unsigned char c = *t;
		const char *e = url_escape_table[c];
		if (e && c != '+' && c != '&') {
			html_raw(txt, t - txt);
			html(e);
			txt = t + 1;
		}
		t++;
	}
	if (t != txt)
		html(txt);
}