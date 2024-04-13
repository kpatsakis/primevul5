static int getlonglong(FILE *sfd, long long *val) {
    char tokbuf[100]; int ch;
    char *pt=tokbuf, *end = tokbuf+100-2;

    while ( isspace(ch = nlgetc(sfd)));
    if ( ch=='-' || ch=='+' ) {
	*pt++ = ch;
	ch = nlgetc(sfd);
    }
    while ( isdigit(ch)) {
	if ( pt<end ) *pt++ = ch;
	ch = nlgetc(sfd);
    }
    *pt='\0';
    ungetc(ch,sfd);
    *val = strtoll(tokbuf,NULL,10);
return( pt!=tokbuf?1:ch==EOF?-1: 0 );
}