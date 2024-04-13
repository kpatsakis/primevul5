char **NamesReadSFD(char *filename) {
    FILE *sfd = fopen(filename,"r");
    char tok[2000];
    char **ret = NULL;
    int eof;

    if ( sfd==NULL )
return( NULL );
    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.
    if ( SFDStartsCorrectly(sfd,tok)!=-1 ) {
	while ( !feof(sfd)) {
	    if ( (eof = getname(sfd,tok))!=1 ) {
		if ( eof==-1 )
	break;
		geteol(sfd,tok);
	continue;
	    }
	    if ( strmatch(tok,"FontName:")==0 ) {
		getname(sfd,tok);
		ret = malloc(2*sizeof(char*));
		ret[0] = copy(tok);
		ret[1] = NULL;
	break;
	    }
	}
    }
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
    fclose(sfd);
return( ret );
}