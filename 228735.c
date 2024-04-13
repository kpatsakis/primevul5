static void putstring(FILE *sfd, const char *header, char *body) {
    fprintf( sfd, "%s", header );
    while ( *body ) {
	if ( *body=='\n' || *body == '\\' || *body=='\r' ) {
	    putc('\\',sfd);
	    if ( *body=='\\' )
		putc('\\',sfd);
	    else {
		putc('n',sfd);
		if ( *body=='\r' && body[1]=='\n' )
		    ++body;
	    }
	} else
	    putc(*body,sfd);
	++body;
    }
    putc('\n',sfd);
}