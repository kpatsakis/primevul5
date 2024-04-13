static struct basescript *SFDParseBaseScript(FILE *sfd,struct Base *base) {
    struct basescript *bs;
    int i, ch;
    struct baselangextent *last, *cur;

    if ( base==NULL )
return(NULL);

    bs = chunkalloc(sizeof(struct basescript));

    bs->script = gettag(sfd);
    getint(sfd,&bs->def_baseline);
    if ( base->baseline_cnt!=0 ) {
	bs->baseline_pos = calloc(base->baseline_cnt,sizeof(int16));
	for ( i=0; i<base->baseline_cnt; ++i )
	    getsint(sfd, &bs->baseline_pos[i]);
    }
    while ( (ch=nlgetc(sfd))==' ' );
    last = NULL;
    while ( ch=='{' ) {
	ungetc(ch,sfd);
	cur = ParseBaseLang(sfd);
	if ( last==NULL )
	    bs->langs = cur;
	else
	    last->next = cur;
	last = cur;
	while ( (ch=nlgetc(sfd))==' ' );
    }
return( bs );
}