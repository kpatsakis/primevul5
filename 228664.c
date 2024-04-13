static Encoding *SFDGetEncoding(FILE *sfd, char *tok) {
    Encoding *enc = NULL;
    int encname;

    if ( getint(sfd,&encname) ) {
	if ( encname<(int)(sizeof(charset_names)/sizeof(charset_names[0])-1) )
	    enc = FindOrMakeEncoding(charset_names[encname]);
    } else {
	geteol(sfd,tok);
	enc = FindOrMakeEncoding(tok);
    }
    if ( enc==NULL )
	enc = &custom;
return( enc );
}