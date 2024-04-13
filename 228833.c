static void SFDDumpGasp(FILE *sfd, SplineFont *sf) {
    int i;

    if ( sf->gasp_cnt==0 )
return;

    fprintf( sfd, "GaspTable: %d", sf->gasp_cnt );
    for ( i=0; i<sf->gasp_cnt; ++i )
	fprintf( sfd, " %d %d", sf->gasp[i].ppem, sf->gasp[i].flags );
    fprintf( sfd, " %d", sf->gasp_version);
    putc('\n',sfd);
}