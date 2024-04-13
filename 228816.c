static struct ttf_table *SFDGetShortTable(FILE *sfd, SplineFont *sf,struct ttf_table *lasttab[2]) {
    /* We've read the ShortTable token, it is followed by a tag and a word count */
    /* and then the (text) values of the words that make up the cvt table */
    int i,len, ch;
    uint8 *pt;
    int which, iscvt, started;
    struct ttf_table *tab = chunkalloc(sizeof(struct ttf_table));

    tab->tag = gettag(sfd);

    if ( tab->tag==CHR('f','p','g','m') || tab->tag==CHR('p','r','e','p') ||
	    tab->tag==CHR('c','v','t',' ') || tab->tag==CHR('m','a','x','p'))
	which = 0;
    else
	which = 1;
    iscvt = tab->tag==CHR('c','v','t',' ');

    getint(sfd,&len);
    pt = tab->data = malloc(2*len);
    tab->len = 2*len;
    started = false;
    for ( i=0; i<len; ++i ) {
	int num;
	getint(sfd,&num);
	*pt++ = num>>8;
	*pt++ = num&0xff;
	if ( iscvt ) {
	    ch = nlgetc(sfd);
	    if ( ch==' ' ) {
		if ( !started ) {
		    sf->cvt_names = calloc(len+1,sizeof(char *));
		    sf->cvt_names[len] = END_CVT_NAMES;
		    started = true;
		}
		sf->cvt_names[i] = SFDReadUTF7Str(sfd);
	    } else
		ungetc(ch,sfd);
	}
    }

    if ( lasttab[which]!=NULL )
	lasttab[which]->next = tab;
    else if ( which==0 )
	sf->ttf_tables = tab;
    else
	sf->ttf_tab_saved = tab;
    lasttab[which] = tab;
return( tab );
}