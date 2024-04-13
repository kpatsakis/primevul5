static struct glyphvariants *SFDParseGlyphComposition(FILE *sfd,
	struct glyphvariants *gv, char *tok) {
    int i;

    if ( gv==NULL )
	gv = chunkalloc(sizeof(struct glyphvariants));
    getint(sfd,&gv->part_cnt);
    gv->parts = calloc(gv->part_cnt,sizeof(struct gv_part));
    for ( i=0; i<gv->part_cnt; ++i ) {
	int temp, ch;
	getname(sfd,tok);
	gv->parts[i].component = copy(tok);
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!='%' ) ungetc(ch,sfd);
	getint(sfd,&temp);
	gv->parts[i].is_extender = temp;
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!=',' ) ungetc(ch,sfd);
	getint(sfd,&temp);
	gv->parts[i].startConnectorLength=temp;
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!=',' ) ungetc(ch,sfd);
	getint(sfd,&temp);
	gv->parts[i].endConnectorLength = temp;
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!=',' ) ungetc(ch,sfd);
	getint(sfd,&temp);
	gv->parts[i].fullAdvance = temp;
    }
return( gv );
}