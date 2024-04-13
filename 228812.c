static void SFGuessScriptList(SplineFont1 *sf) {
    uint32 scripts[32], script;
    int i, scnt=0, j;

    for ( i=0; i<sf->sf.glyphcnt; ++i ) if ( sf->sf.glyphs[i]!=NULL ) {
	script = SCScriptFromUnicode(sf->sf.glyphs[i]);
	if ( script!=0 && script!=DEFAULT_SCRIPT ) {
	    for ( j=scnt-1; j>=0 ; --j )
		if ( scripts[j]==script )
	    break;
	    if ( j<0 ) {
		scripts[scnt++] = script;
		if ( scnt>=32 )
    break;
	    }
	}
    }
    if ( scnt==0 )
	scripts[scnt++] = CHR('l','a','t','n');

    /* order scripts */
    for ( i=0; i<scnt-1; ++i ) for ( j=i+1; j<scnt; ++j ) {
	if ( scripts[i]>scripts[j] ) {
	    script = scripts[i];
	    scripts[i] = scripts[j];
	    scripts[j] = script;
	}
    }

    if ( sf->sf.cidmaster ) sf = (SplineFont1 *) sf->sf.cidmaster;
    else if ( sf->sf.mm!=NULL ) sf=(SplineFont1 *) sf->sf.mm->normal;
    if ( sf->script_lang!=NULL )
return;
    sf->script_lang = calloc(2,sizeof(struct script_record *));
    sf->script_lang[0] = calloc(scnt+1,sizeof(struct script_record));
    sf->sli_cnt = 1;
    for ( j=0; j<scnt; ++j ) {
	sf->script_lang[0][j].script = scripts[j];
	sf->script_lang[0][j].langs = malloc(2*sizeof(uint32));
	sf->script_lang[0][j].langs[0] = DEFAULT_LANG;
	sf->script_lang[0][j].langs[1] = 0;
    }
    sf->script_lang[1] = NULL;
}