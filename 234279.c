char *convert_query_hexchar(char *txt)
{
	int d1, d2;
	if (strlen(txt) < 3) {
		*txt = '\0';
		return txt-1;
	}
	d1 = hextoint(*(txt+1));
	d2 = hextoint(*(txt+2));
	if (d1<0 || d2<0) {
		strcpy(txt, txt+3);
		return txt-1;
	} else {
		*txt = d1 * 16 + d2;
		strcpy(txt+1, txt+3);
		return txt;
	}
}