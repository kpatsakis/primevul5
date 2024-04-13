static int gethex(FILE *sfd, uint32 *val) {
    char tokbuf[100]; int ch;
    char *pt=tokbuf, *end = tokbuf+100-2;

    while ( isspace(ch = nlgetc(sfd)));
    if ( ch=='#' )
	ch = nlgetc(sfd);
    if ( ch=='-' || ch=='+' ) {
	*pt++ = ch;
	ch = nlgetc(sfd);
    }
    if ( ch=='0' ) {
	ch = nlgetc(sfd);
	if ( ch=='x' || ch=='X' )
	    ch = nlgetc(sfd);
	else {
	    ungetc(ch,sfd);
	    ch = '0';
	}
    }
    while ( isdigit(ch) || (ch>='a' && ch<='f') || (ch>='A' && ch<='F')) {
	if ( pt<end ) *pt++ = ch;
	ch = nlgetc(sfd);
    }
    *pt='\0';
    ungetc(ch,sfd);
    *val = strtoul(tokbuf,NULL,16);
return( pt!=tokbuf?1:ch==EOF?-1: 0 );
}