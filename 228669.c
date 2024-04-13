static struct Base *SFDParseBase(FILE *sfd) {
    struct Base *base = chunkalloc(sizeof(struct Base));
    int i;

    getint(sfd,&base->baseline_cnt);
    if ( base->baseline_cnt!=0 ) {
	base->baseline_tags = malloc(base->baseline_cnt*sizeof(uint32));
	for ( i=0; i<base->baseline_cnt; ++i )
	    base->baseline_tags[i] = gettag(sfd);
    }
return( base );
}