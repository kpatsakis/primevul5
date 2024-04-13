static FeatureScriptLangList *FeaturesFromTagSli(uint32 tag,int sli,SplineFont1 *sf) {
    FeatureScriptLangList *fl;
    struct script_record *sr;
    struct scriptlanglist *cur, *last;
    int i;

    fl = chunkalloc(sizeof(FeatureScriptLangList));
    fl->featuretag = tag;
    if ( sli==SLI_NESTED || sli<0 || sli>=sf->sli_cnt )
return( fl );
    last = NULL;
    for ( sr = sf->script_lang[sli]; sr->script!=0; ++sr ) {
	cur = chunkalloc(sizeof(struct scriptlanglist));
	cur->script = sr->script;
	for ( i=0; sr->langs[i]!=0; ++i );
	cur->lang_cnt = i;
	if ( i>MAX_LANG )
	    cur->morelangs = malloc((i-MAX_LANG) * sizeof(uint32));
	for ( i=0; sr->langs[i]!=0; ++i ) {
	    if ( i<MAX_LANG )
		cur->langs[i] = sr->langs[i];
	    else
		cur->morelangs[i-MAX_LANG] = sr->langs[i];
	}
	if ( last==NULL )
	    fl->scripts = cur;
	else
	    last->next = cur;
	last = cur;
    }
return( fl );
}