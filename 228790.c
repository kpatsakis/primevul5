static int SFAddScriptLangIndex(SplineFont *_sf,uint32 script,uint32 lang) {
    int i;
    SplineFont1 *sf;

    if ( _sf->cidmaster ) _sf = _sf->cidmaster;
    else if ( _sf->mm!=NULL ) _sf=_sf->mm->normal;

    if ( _sf->sfd_version>=2 )
	IError( "SFFindBiggestScriptLangIndex called with bad version number.\n" );

    sf = (SplineFont1 *) _sf;

    if ( script==0 ) script=DEFAULT_SCRIPT;
    if ( lang==0 ) lang=DEFAULT_LANG;
    if ( sf->script_lang==NULL )
	sf->script_lang = calloc(2,sizeof(struct script_record *));
    for ( i=0; sf->script_lang[i]!=NULL; ++i ) {
	if ( sf->script_lang[i][0].script==script && sf->script_lang[i][1].script==0 &&
		sf->script_lang[i][0].langs[0]==lang &&
		sf->script_lang[i][0].langs[1]==0 )
return( i );
    }
    sf->script_lang = realloc(sf->script_lang,(i+2)*sizeof(struct script_record *));
    sf->script_lang[i] = calloc(2,sizeof(struct script_record));
    sf->script_lang[i][0].script = script;
    sf->script_lang[i][0].langs = malloc(2*sizeof(uint32));
    sf->script_lang[i][0].langs[0] = lang;
    sf->script_lang[i][0].langs[1] = 0;
    sf->script_lang[i+1] = NULL;
    sf->sli_cnt = i+1;
return( i );
}