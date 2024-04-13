static OTLookup **SFDLookupList(FILE *sfd,SplineFont *sf) {
    int ch;
    OTLookup **ret=NULL, *otl;
    int lcnt=0, lmax=0;
    char *name;

    for (;;) {
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch=='\n' || ch==EOF )
    break;
	ungetc(ch,sfd);
	name = SFDReadUTF7Str(sfd);
	otl = SFFindLookup(sf,name);
	free(name);
	if ( otl!=NULL ) {
	    if ( lcnt>=lmax ) {
	        lmax += 100;
	        ret = realloc(ret, lmax * sizeof(OTLookup *));
	    }
	    ret[lcnt++] = otl;
	}
    }
    if ( lcnt==0 )
return( NULL );
    ret = realloc(ret, (lcnt+1) * sizeof(OTLookup *));
    ret[lcnt] = NULL;
return( ret );
}