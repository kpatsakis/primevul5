static void SFDDumpBaseLang(FILE *sfd,struct baselangextent *bl) {

    if ( bl->lang==0 )
	fprintf( sfd, " { %d %d", bl->descent, bl->ascent );
    else
	fprintf( sfd, " { '%c%c%c%c' %d %d",
		bl->lang>>24, bl->lang>>16, bl->lang>>8, bl->lang,
		bl->descent, bl->ascent );
    for ( bl=bl->features; bl!=NULL; bl=bl->next )
	SFDDumpBaseLang(sfd,bl);
    putc('}',sfd);
}