static void SFDFpstClassNamesOut(FILE *sfd,int class_cnt,char **classnames,const char *keyword) {
    char buffer[20];
    int i;

    if ( class_cnt>0 && classnames!=NULL ) {
	fprintf( sfd, "  %s: ", keyword );
	for ( i=0; i<class_cnt; ++i ) {
	    if ( classnames[i]==NULL ) {
		sprintf( buffer,"%d", i );
		SFDDumpUTF7Str(sfd,buffer);
	    } else
		SFDDumpUTF7Str(sfd,classnames[i]);
	    if ( i<class_cnt-1 ) putc(' ',sfd);
	}
	putc('\n',sfd);
    }
}