static void SFDDumpTtfInstrsExplicit(FILE *sfd,uint8 *ttf_instrs, int16 ttf_instrs_len )
{
    char *instrs = _IVUnParseInstrs( ttf_instrs, ttf_instrs_len );
    char *pt;
    fprintf( sfd, "TtInstrs:\n" );
    for ( pt=instrs; *pt!='\0'; ++pt )
	putc(*pt,sfd);
    if ( pt[-1]!='\n' )
	putc('\n',sfd);
    free(instrs);
    fprintf( sfd, "%s\n", end_tt_instrs );
}