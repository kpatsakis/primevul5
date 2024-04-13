static int geteol(FILE *sfd, char *tokbuf) {
    char *pt=tokbuf, *end = tokbuf+2000-2; int ch;

    while ( isspace(ch = nlgetc(sfd)) && ch!='\r' && ch!='\n' );
    while ( ch!='\n' && ch!='\r' && ch!=EOF ) {
	if ( pt<end ) *pt++ = ch;
	ch = nlgetc(sfd);
    }
    *pt='\0';
return( pt!=tokbuf?1:ch==EOF?-1: 0 );
}