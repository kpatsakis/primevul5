int SFAddScriptIndex(SplineFont1 *sf,uint32 *scripts,int scnt) {
    int i,j;
    struct script_record *sr;

    if ( scnt==0 )
	scripts[scnt++] = CHR('l','a','t','n');		/* Need a default script preference */
    for ( i=0; i<scnt-1; ++i ) for ( j=i+1; j<scnt; ++j ) {
	if ( scripts[i]>scripts[j] ) {
	    uint32 temp = scripts[i];
	    scripts[i] = scripts[j];
	    scripts[j] = temp;
	}
    }

    if ( sf->sf.cidmaster ) sf = (SplineFont1 *) sf->sf.cidmaster;
    if ( sf->script_lang==NULL )	/* It's an old sfd file */
	sf->script_lang = calloc(1,sizeof(struct script_record *));
    for ( i=0; sf->script_lang[i]!=NULL; ++i ) {
	sr = sf->script_lang[i];
	for ( j=0; sr[j].script!=0 && j<scnt &&
		sr[j].script==scripts[j]; ++j );
	if ( sr[j].script==0 && j==scnt )
return( i );
    }

    sf->script_lang = realloc(sf->script_lang,(i+2)*sizeof(struct script_record *));
    sf->script_lang[i+1] = NULL;
    sr = sf->script_lang[i] = calloc(scnt+1,sizeof(struct script_record));
    for ( j = 0; j<scnt; ++j ) {
	sr[j].script = scripts[j];
	sr[j].langs = malloc(2*sizeof(uint32));
	sr[j].langs[0] = DEFAULT_LANG;
	sr[j].langs[1] = 0;
    }
return( i );
}