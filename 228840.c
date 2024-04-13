static struct ttf_table *SFDGetTtTable(FILE *sfd, SplineFont *sf,struct ttf_table *lasttab[2]) {
    /* We've read the TtTable token, it is followed by a tag */
    /* and then the instructions in text format */
    int ch;
    int which;
    struct ttf_table *tab = chunkalloc(sizeof(struct ttf_table));
    char *buf=NULL, *pt=buf, *end=buf;
    int backlen = strlen(end_tt_instrs);

    tab->tag = gettag(sfd);

    if ( tab->tag==CHR('f','p','g','m') || tab->tag==CHR('p','r','e','p') ||
	    tab->tag==CHR('c','v','t',' ') || tab->tag==CHR('m','a','x','p'))
	which = 0;
    else
	which = 1;

    while ( (ch=nlgetc(sfd))!=EOF ) {
	if ( pt>=end ) {
	    char *newbuf = realloc(buf,(end-buf+200));
	    pt = newbuf+(pt-buf);
	    end = newbuf+(end+200-buf);
	    buf = newbuf;
	}
	*pt++ = ch;
	if ( pt-buf>backlen && strncmp(pt-backlen,end_tt_instrs,backlen)==0 ) {
	    pt -= backlen;
    break;
	}
    }
    *pt = '\0';
    tab->data = _IVParse(sf,buf,&tab->len,tterr,NULL);
    free(buf);

    if ( lasttab[which]!=NULL )
	lasttab[which]->next = tab;
    else if ( which==0 )
	sf->ttf_tables = tab;
    else
	sf->ttf_tab_saved = tab;
    lasttab[which] = tab;
return( tab );
}