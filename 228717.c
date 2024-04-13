static DeviceTable *SFDReadDeviceTable(FILE *sfd,DeviceTable *adjust) {
    int i, junk, first, last, ch, len;

    while ( (ch=nlgetc(sfd))==' ' );
    if ( ch=='{' ) {
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch=='}' )
return(NULL);
	else
	    ungetc(ch,sfd);
	getint(sfd,&first);
	ch = nlgetc(sfd);		/* Should be '-' */
	getint(sfd,&last);
	len = last-first+1;
	if ( len<=0 ) {
	    IError( "Bad device table, invalid length.\n" );
return(NULL);
	}
	if ( adjust==NULL )
	    adjust = chunkalloc(sizeof(DeviceTable));
	adjust->first_pixel_size = first;
	adjust->last_pixel_size = last;
	adjust->corrections = malloc(len);
	for ( i=0; i<len; ++i ) {
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch!=',' ) ungetc(ch,sfd);
	    getint(sfd,&junk);
	    adjust->corrections[i] = junk;
	}
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!='}' ) ungetc(ch,sfd);
    } else
	ungetc(ch,sfd);
return( adjust );
}