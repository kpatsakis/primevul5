static int gethexints(FILE *sfd, uint32 *val, int cnt) {
    int i, ch;

    for ( i=0; i<cnt; ++i ) {
	if ( i!=0 ) {
	    ch = nlgetc(sfd);
	    if ( ch!='.' ) ungetc(ch,sfd);
	}
	if ( !gethex(sfd,&val[i]))
return( false );
    }
return( true );
}