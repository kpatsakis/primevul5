char *getquotedeol(FILE *sfd) {
    char *pt, *str, *end;
    int ch;

    pt = str = malloc(101); end = str+100;
    while ( isspace(ch = nlgetc(sfd)) && ch!='\r' && ch!='\n' );
    while ( ch!='\n' && ch!='\r' && ch!=EOF ) {
	if ( ch=='\\' ) {
	    /* We can't use nlgetc() here, because it would misinterpret */
	    /* double backslash at the end of line. Multiline strings,   */
	    /* broken with backslash + newline, are just handled above.  */
	    ch = getc(sfd);
	    if ( ch=='n' ) ch='\n';
	    /* else if ( ch=='\\' ) ch=='\\'; */ /* second backslash of '\\' */

	    /* FontForge doesn't write other escape sequences in this context. */
	    /* So any other value of ch is assumed impossible. */
	}
	if ( pt>=end ) {
	    pt = realloc(str,end-str+101);
	    end = pt+(end-str)+100;
	    str = pt;
	    pt = end-100;
	}
	*pt++ = ch;
	ch = nlgetc(sfd);
    }
    *pt='\0';
    /* these strings should be in utf8 now, but some old sfd files might have */
    /* latin1. Not a severe problems because they SHOULD be in ASCII. So any */
    /* non-ascii strings are erroneous anyway */
    if ( !utf8_valid(str) ) {
	pt = latin1_2_utf8_copy(str);
	free(str);
	str = pt;
    }
return( str );
}