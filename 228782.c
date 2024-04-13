static void SFDDumpMathVertex(FILE *sfd,struct mathkernvertex *vert,const char *name) {
    int i;

    if ( vert==NULL || vert->cnt==0 )
return;

    fprintf( sfd, "%s %d ", name, vert->cnt );
    for ( i=0; i<vert->cnt; ++i ) {
	fprintf( sfd, " %d", vert->mkd[i].height );
	SFDDumpDeviceTable(sfd,vert->mkd[i].height_adjusts );
	fprintf( sfd, ",%d", vert->mkd[i].kern );
	SFDDumpDeviceTable(sfd,vert->mkd[i].kern_adjusts );
    }
    putc('\n',sfd );
}