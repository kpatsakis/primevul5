void html_ntxt(int len, char *txt)
{
	char *t = txt;
	while(t && *t && len--){
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
		write(htmlfd, txt, t - txt);
	if (len<0)
		html("...");
}