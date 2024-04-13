void html_ntxt(int len, const char *txt)
{
	const char *t = txt;
	while (t && *t && len--) {
		int c = *t;
		if (c == '<' || c == '>' || c == '&') {
			html_raw(txt, t - txt);
			if (c == '>')
				html("&gt;");
			else if (c == '<')
				html("&lt;");
			else if (c == '&')
				html("&amp;");
			txt = t + 1;
		}
		t++;
	}
	if (t != txt)
		html_raw(txt, t - txt);
	if (len < 0)
		html("...");
}