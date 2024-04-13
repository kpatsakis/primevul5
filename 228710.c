static void SFDDumpJSTFLookups(FILE *sfd,const char *keyword, OTLookup **list ) {
    int i;

    if ( list==NULL || list[0]==NULL )
return;

    fprintf( sfd, "%s ", keyword );
    for ( i=0; list[i]!=NULL; ++i ) {
	SFDDumpUTF7Str(sfd,list[i]->lookup_name);
	if ( list[i+1]!=NULL ) putc(' ',sfd);
    }
    putc('\n',sfd);
}