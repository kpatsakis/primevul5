int SFFindBiggestScriptLangIndex(SplineFont *_sf,uint32 script,uint32 lang) {
    int i, best_sli= -1, best_cnt= -1, cnt;
    SplineFont1 *sf = (SplineFont1 *) _sf;

    if ( _sf->sfd_version>=2 )
	IError( "SFFindBiggestScriptLangIndex called with bad version number.\n" );

    if ( sf->script_lang==NULL )
	SFGuessScriptList(sf);
    for ( i=0; sf->script_lang[i]!=NULL; ++i ) {
	if ( SLContains(sf->script_lang[i],script,lang)) {
	    cnt = SLCount(sf->script_lang[i]);
	    if ( cnt>best_cnt ) {
		best_sli = i;
		best_cnt = cnt;
	    }
	}
    }
    if ( best_sli==-1 )
return( SFAddScriptLangIndex(_sf,script,lang) );

return( best_sli );
}