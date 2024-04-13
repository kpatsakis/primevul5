static void SFDDumpBase(FILE *sfd,const char *keyword,struct Base *base) {
    int i;
    struct basescript *bs;
    struct baselangextent *bl;

    fprintf( sfd, "%s %d", keyword, base->baseline_cnt );
    for ( i=0; i<base->baseline_cnt; ++i ) {
	fprintf( sfd, " '%c%c%c%c'",
		base->baseline_tags[i]>>24,
		base->baseline_tags[i]>>16,
		base->baseline_tags[i]>>8,
		base->baseline_tags[i]);
    }
    putc('\n',sfd);

    for ( bs=base->scripts; bs!=NULL; bs=bs->next ) {
	fprintf( sfd, "BaseScript: '%c%c%c%c' %d ",
		bs->script>>24, bs->script>>16, bs->script>>8, bs->script,
		bs->def_baseline );
	for ( i=0; i<base->baseline_cnt; ++i )
	    fprintf( sfd, " %d", bs->baseline_pos[i]);
	for ( bl=bs->langs; bl!=NULL; bl=bl->next )
	    SFDDumpBaseLang(sfd,bl);
	putc('\n',sfd);
    }
}