static struct baselangextent *ParseBaseLang(FILE *sfd) {
    struct baselangextent *bl;
    struct baselangextent *cur, *last;
    int ch;

    while ( (ch=nlgetc(sfd))==' ' );
    if ( ch=='{' ) {
	bl = chunkalloc(sizeof(struct baselangextent));
	while ( (ch=nlgetc(sfd))==' ' );
	ungetc(ch,sfd);
	if ( ch=='\'' )
	    bl->lang = gettag(sfd);		/* Lang or Feature tag, or nothing */
	getsint(sfd,&bl->descent);
	getsint(sfd,&bl->ascent);
	last = NULL;
	while ( (ch=nlgetc(sfd))==' ' );
	while ( ch=='{' ) {
	    ungetc(ch,sfd);
	    cur = ParseBaseLang(sfd);
	    if ( last==NULL )
		bl->features = cur;
	    else
		last->next = cur;
	    last = cur;
	    while ( (ch=nlgetc(sfd))==' ' );
	}
	if ( ch!='}' ) ungetc(ch,sfd);
return( bl );
    }
return( NULL );
}