static void SFDParseJustify(FILE *sfd, SplineFont *sf, char *tok) {
    Justify *last=NULL, *cur;
    struct jstf_lang *jlang, *llast;
    int p = 0,ch;

    while ( strcmp(tok,"Justify:")==0 ) {
	cur = chunkalloc(sizeof(Justify));
	if ( last==NULL )
	    sf->justify = cur;
	else
	    last->next = cur;
	last = cur;
	llast = jlang = NULL;
	cur->script = gettag(sfd);
	while ( getname(sfd,tok)>0 ) {
	    if ( strcmp(tok,"Justify:")==0 || strcmp(tok,"EndJustify")==0 )
	break;
	    if ( strcmp(tok,"JstfExtender:")==0 ) {
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		geteol(sfd,tok);
		cur->extenders = copy(tok);
	    } else if ( strcmp(tok,"JstfLang:")==0 ) {
		jlang = chunkalloc(sizeof(struct jstf_lang));
		if ( llast==NULL )
		    cur->langs = jlang;
		else
		    llast->next = jlang;
		llast = jlang;
		jlang->lang = gettag(sfd);
		p = -1;
		getint(sfd,&jlang->cnt);
		if ( jlang->cnt!=0 )
		    jlang->prios = calloc(jlang->cnt,sizeof(struct jstf_prio));
	    } else if ( strcmp(tok,"JstfPrio:")==0 ) {
		if ( jlang!=NULL ) {
		    ++p;
		    if ( p>= jlang->cnt ) {
			jlang->prios = realloc(jlang->prios,(p+1)*sizeof(struct jstf_prio));
			memset(jlang->prios+jlang->cnt,0,(p+1-jlang->cnt)*sizeof(struct jstf_prio));
			jlang->cnt = p+1;
		    }
		}
	    } else if ( strcmp(tok,"JstfEnableShrink:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].enableShrink = SFDLookupList(sfd,sf);
	    } else if ( strcmp(tok,"JstfDisableShrink:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].disableShrink = SFDLookupList(sfd,sf);
	    } else if ( strcmp(tok,"JstfMaxShrink:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].maxShrink = SFDLookupList(sfd,sf);
	    } else if ( strcmp(tok,"JstfEnableExtend:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].enableExtend = SFDLookupList(sfd,sf);
	    } else if ( strcmp(tok,"JstfDisableExtend:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].disableExtend = SFDLookupList(sfd,sf);
	    } else if ( strcmp(tok,"JstfMaxExtend:" )==0 ) {
		if ( p<0 ) p=0;
		if ( jlang!=NULL && p<jlang->cnt )
		    jlang->prios[p].maxExtend = SFDLookupList(sfd,sf);
	    } else
		geteol(sfd,tok);
	}
    }
}