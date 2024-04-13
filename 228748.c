static void SFDDumpDeviceTable(FILE *sfd,DeviceTable *adjust) {
    int i;

    if ( adjust==NULL )
return;
    fprintf( sfd, "{" );
    if ( adjust->corrections!=NULL ) {
	fprintf( sfd, "%d-%d ", adjust->first_pixel_size, adjust->last_pixel_size );
	for ( i=0; i<=adjust->last_pixel_size-adjust->first_pixel_size; ++i )
	    fprintf( sfd, "%s%d", i==0?"":",", adjust->corrections[i]);
    }
    fprintf( sfd, "}" );
}