static void SFDCleanupAnchorClasses(SplineFont *sf) {
    AnchorClass *ac;
    AnchorPoint *ap;
    int i, j, scnt;
#define S_MAX	100
    uint32 scripts[S_MAX];
    int merge=0;

    for ( ac = sf->anchor; ac!=NULL; ac=ac->next ) {
	if ( ((AnchorClass1 *) ac)->script_lang_index==0xffff ) {
	    scnt = 0;
	    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
		for ( ap = sf->glyphs[i]->anchor; ap!=NULL && ap->anchor!=ac; ap=ap->next );
		if ( ap!=NULL && scnt<S_MAX ) {
		    uint32 script = SCScriptFromUnicode(sf->glyphs[i]);
		    if ( script==0 )
	    continue;
		    for ( j=0; j<scnt; ++j )
			if ( scripts[j]==script )
		    break;
		    if ( j==scnt )
			scripts[scnt++] = script;
		}
	    }
	    ((AnchorClass1 *) ac)->script_lang_index = SFAddScriptIndex((SplineFont1 *) sf,scripts,scnt);
	}
	if ( ((AnchorClass1 *) ac)->merge_with == 0xffff )
	    ((AnchorClass1 *) ac)->merge_with = ++merge;
    }
#undef S_MAX
}