MacFeat *SFDParseMacFeatures(FILE *sfd, char *tok) {
    MacFeat *cur, *head=NULL, *last=NULL;
    struct macsetting *slast, *scur;
    int feat, ism, def, set;

    while ( strcmp(tok,"MacFeat:")==0 ) {
	cur = chunkalloc(sizeof(MacFeat));
	if ( last==NULL )
	    head = cur;
	else
	    last->next = cur;
	last = cur;

	getint(sfd,&feat); getint(sfd,&ism); getint(sfd, &def);
	cur->feature = feat; cur->ismutex = ism; cur->default_setting = def;
	getname(sfd,tok);
	cur->featname = SFDParseMacNames(sfd,tok);
	slast = NULL;
	while ( strcmp(tok,"MacSetting:")==0 ) {
	    scur = chunkalloc(sizeof(struct macsetting));
	    if ( slast==NULL )
		cur->settings = scur;
	    else
		slast->next = scur;
	    slast = scur;

	    getint(sfd,&set);
	    scur->setting = set;
	    getname(sfd,tok);
	    scur->setname = SFDParseMacNames(sfd,tok);
	}
    }
return( head );
}