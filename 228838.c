static int getreal(FILE *sfd, real *val) {
    char tokbuf[100];
    int ch;
    char *pt=tokbuf, *end = tokbuf+100-2, *nend;

    while ( isspace(ch = nlgetc(sfd)));
    if ( ch!='e' && ch!='E' )		/* real's can't begin with exponants */
	while ( isdigit(ch) || ch=='-' || ch=='+' || ch=='e' || ch=='E' || ch=='.' || ch==',' ) {
	    if ( pt<end ) *pt++ = ch;
	    ch = nlgetc(sfd);
	}
    *pt='\0';
    ungetc(ch,sfd);
    *val = strtod(tokbuf,&nend);
    /* Beware of different locals! */
    if ( *nend!='\0' ) {
	if ( *nend=='.' )
	    *nend = ',';
	else if ( *nend==',' )
	    *nend = '.';
	*val = strtod(tokbuf,&nend);
    }
return( pt!=tokbuf && *nend=='\0'?1:ch==EOF?-1: 0 );
}