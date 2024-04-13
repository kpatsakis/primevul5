static int getint(FILE *sfd, int *val) {
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
    *val = strtol(tokbuf,NULL,10);
return( pt!=tokbuf?1:ch==EOF?-1: 0 );
}