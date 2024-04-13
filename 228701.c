static void SFDDumpMacName(FILE *sfd,struct macname *mn) {
    char *pt;

    while ( mn!=NULL ) {
      fprintf( sfd, "MacName: %d %d %d \"", mn->enc, mn->lang,
	       (int)strlen(mn->name) );
	for ( pt=mn->name; *pt; ++pt ) {
	    if ( *pt<' ' || *pt>=0x7f || *pt=='\\' || *pt=='"' )
		fprintf( sfd, "\\%03o", *(uint8 *) pt );
	    else
		putc(*pt,sfd);
	}
	fprintf( sfd, "\"\n" );
	mn = mn->next;
    }
}