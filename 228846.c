static void SFDDumpJustify(FILE *sfd,SplineFont *sf) {
    Justify *jscript;
    struct jstf_lang *jlang;
    int i;

    for ( jscript = sf->justify; jscript!=NULL; jscript=jscript->next ) {
	fprintf( sfd, "Justify: '%c%c%c%c'\n",
		jscript->script>>24,
		jscript->script>>16,
		jscript->script>>8,
		jscript->script);
	if ( jscript->extenders!=NULL )
	    fprintf( sfd, "JstfExtender: %s\n", jscript->extenders );
	for ( jlang = jscript->langs; jlang!=NULL; jlang = jlang->next ) {
	    fprintf( sfd, "JstfLang: '%c%c%c%c' %d\n",
		jlang->lang>>24,
		jlang->lang>>16,
		jlang->lang>>8,
		jlang->lang, jlang->cnt );
	    for ( i=0; i<jlang->cnt; ++i ) {
		fprintf( sfd, "JstfPrio:\n" );
		SFDDumpJSTFLookups(sfd,"JstfEnableShrink:", jlang->prios[i].enableShrink );
		SFDDumpJSTFLookups(sfd,"JstfDisableShrink:", jlang->prios[i].disableShrink );
		SFDDumpJSTFLookups(sfd,"JstfMaxShrink:", jlang->prios[i].maxShrink );
		SFDDumpJSTFLookups(sfd,"JstfEnableExtend:", jlang->prios[i].enableExtend );
		SFDDumpJSTFLookups(sfd,"JstfDisableExtend:", jlang->prios[i].disableExtend );
		SFDDumpJSTFLookups(sfd,"JstfMaxExtend:", jlang->prios[i].maxExtend );
	    }
	}
    }
    if ( sf->justify!=NULL )
	fprintf( sfd, "EndJustify\n" );
}