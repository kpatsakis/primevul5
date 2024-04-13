static OTLookup *CreateMacLookup(SplineFont1 *sf,ASM1 *sm) {
    OTLookup *otl = chunkalloc(sizeof(OTLookup));
    int i, ch;
    char *pt, *start;
    SplineChar *sc;

    otl->features = chunkalloc(sizeof(FeatureScriptLangList));
    if ( sm->sm.type == asm_kern ) {
	otl->lookup_type = kern_statemachine;
	otl->next = sf->sf.gpos_lookups;
	sf->sf.gpos_lookups = otl;
	otl->features->featuretag = (sm->sm.flags&0x8000) ? CHR('v','k','r','n') : CHR('k','e','r','n');
    } else {
	otl->lookup_type = sm->sm.type==asm_indic ? morx_indic : sm->sm.type==asm_context ? morx_context : morx_insert;
	otl->next = sf->sf.gsub_lookups;
	sf->sf.gsub_lookups = otl;
	otl->features->featuretag = (sm->feature<<16) | (sm->setting);
	otl->features->ismac = true;
    }
    otl->lookup_flags = 0;

    for ( i=4; i<sm->sm.class_cnt; ++i ) {
	for ( start=sm->sm.classes[i]; ; start = pt ) {
	    while ( *start==' ' ) ++start;
	    if ( *start=='\0' )
	break;
	    for ( pt=start ; *pt!='\0' && *pt!=' '; ++pt );
	    ch = *pt; *pt = '\0';
	    sc = SFGetChar(&sf->sf,-1,start);
	    if ( sc!=NULL )
		FListAppendScriptLang(otl->features,SCScriptFromUnicode(sc),
			DEFAULT_LANG);
	    *pt = ch;
	}
    }
    
    /* We will set the lookup_index after we've ordered the list */
    /* We will set the lookup_name after we've assigned the index */
    /* We will add one subtable soon */
return( otl );
}