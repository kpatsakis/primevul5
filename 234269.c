void html_txt(char *txt)
{
	char *t = txt;
	while(t && *t){
		int c = *t;
		if (c=='<' || c=='>' || c=='&') {
			write(htmlfd, txt, t - txt);
			if (c=='>')
				html("&gt;");
			else if (c=='<')
				html("&lt;");
			else if (c=='&')
				html("&amp;");
			txt = t+1;
		}
		t++;
	}
	if (t!=txt)
		html(txt);
}