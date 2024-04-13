static int getprotectedname(FILE *sfd, char *tokbuf) {
    char *pt=tokbuf, *end = tokbuf+100-2; int ch;

    while ( (ch = nlgetc(sfd))==' ' || ch=='\t' );
    while ( ch!=EOF && !isspace(ch) && ch!='[' && ch!=']' && ch!='{' && ch!='}' && ch!='<' && ch!='%' ) {
	if ( pt<end ) *pt++ = ch;
	ch = nlgetc(sfd);
    }
    if ( pt==tokbuf && ch!=EOF )
	*pt++ = ch;
    else
	ungetc(ch,sfd);
    *pt='\0';
return( pt!=tokbuf?1:ch==EOF?-1: 0 );
}