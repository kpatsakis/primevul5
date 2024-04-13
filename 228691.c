static void SFDDumpTtfTable(FILE *sfd,struct ttf_table *tab,SplineFont *sf) {
    if ( tab->tag == CHR('p','r','e','p') || tab->tag == CHR('f','p','g','m') ) {
	/* These are tables of instructions and should be dumped as such */
	char *instrs;
	char *pt;
	fprintf( sfd, "TtTable: %c%c%c%c\n",
		(int) (tab->tag>>24), (int) ((tab->tag>>16)&0xff), (int) ((tab->tag>>8)&0xff), (int) (tab->tag&0xff) );
	instrs = _IVUnParseInstrs( tab->data,tab->len );
	for ( pt=instrs; *pt!='\0'; ++pt )
	    putc(*pt,sfd);
	if ( pt[-1]!='\n' )
	    putc('\n',sfd);
	free(instrs);
	fprintf( sfd, "%s\n", end_tt_instrs );
    } else if ( (tab->tag == CHR('c','v','t',' ') || tab->tag == CHR('m','a','x','p')) &&
	    (tab->len&1)==0 ) {
	int i, ended;
	uint8 *pt;
	fprintf( sfd, "ShortTable: %c%c%c%c %d\n",
		(int) (tab->tag>>24), (int) ((tab->tag>>16)&0xff), (int) ((tab->tag>>8)&0xff), (int) (tab->tag&0xff),
		(int) (tab->len>>1) );
	pt = (uint8*) tab->data;
	ended = tab->tag!=CHR('c','v','t',' ') || sf->cvt_names==NULL;
	for ( i=0; i<(tab->len>>1); ++i ) {
	    int num = (int16) ((pt[0]<<8) | pt[1]);
	    fprintf( sfd, "  %d", num );
	    if ( !ended ) {
		if ( sf->cvt_names[i]==END_CVT_NAMES )
		    ended=true;
		else if ( sf->cvt_names[i]!=NULL ) {
		    putc(' ',sfd);
		    SFDDumpUTF7Str(sfd,sf->cvt_names[i]);
		    putc(' ',sfd);
		}
	    }
	    putc('\n',sfd);
	    pt += 2;
	}
	fprintf( sfd, "EndShort\n");
    } else {
	/* maxp, who knows what. Dump 'em as binary for now */
	struct enc85 enc;
	int i;

	memset(&enc,'\0',sizeof(enc));
	enc.sfd = sfd;

	fprintf( sfd, "TtfTable: %c%c%c%c %d\n",
		(int) (tab->tag>>24), (int) ((tab->tag>>16)&0xff), (int) ((tab->tag>>8)&0xff), (int) (tab->tag&0xff),
		(int) tab->len );
	for ( i=0; i<tab->len; ++i )
	    SFDEnc85(&enc,tab->data[i]);
	SFDEnc85EndEnc(&enc);
	fprintf(sfd,"\nEndTtf\n" );
    }
}