static struct ttf_table *SFDGetTtfTable(FILE *sfd, SplineFont *sf,struct ttf_table *lasttab[2]) {
    /* We've read the TtfTable token, it is followed by a tag and a byte count */
    /* and then the instructions in enc85 format */
    int i,len;
    int which;
    struct enc85 dec;
    struct ttf_table *tab = chunkalloc(sizeof(struct ttf_table));

    memset(&dec,'\0', sizeof(dec)); dec.pos = -1;
    dec.sfd = sfd;

    tab->tag = gettag(sfd);

    if ( tab->tag==CHR('f','p','g','m') || tab->tag==CHR('p','r','e','p') ||
	    tab->tag==CHR('c','v','t',' ') || tab->tag==CHR('m','a','x','p'))
	which = 0;
    else
	which = 1;

    getint(sfd,&len);
    tab->data = malloc(len);
    tab->len = len;
    for ( i=0; i<len; ++i )
	tab->data[i] = Dec85(&dec);

    if ( lasttab[which]!=NULL )
	lasttab[which]->next = tab;
    else if ( which==0 )
	sf->ttf_tables = tab;
    else
	sf->ttf_tab_saved = tab;
    lasttab[which] = tab;
return( tab );
}