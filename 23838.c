void html_attr(const char *txt)
{
	const char *t = txt;
	while (t && *t) {
		int c = *t;
		if (c == '<' || c == '>' || c == '\'' || c == '\"' || c == '&') {
			html_raw(txt, t - txt);
			if (c == '>')
				html("&gt;");
			else if (c == '<')
				html("&lt;");
			else if (c == '\'')
				html("&#x27;");
			else if (c == '"')
				html("&quot;");
			else if (c == '&')
				html("&amp;");
			txt = t + 1;
		}
		t++;
	}
	if (t != txt)
		html(txt);
}