static OTLookup *CreateACLookup(SplineFont1 *sf,AnchorClass1 *ac) {
    OTLookup *otl = chunkalloc(sizeof(OTLookup));

    otl->lookup_type =
	    ac->ac.type == act_mark ? gpos_mark2base :
	    ac->ac.type == act_mkmk ? gpos_mark2mark :
	    ac->ac.type == act_curs ? gpos_cursive :
	    ac->ac.type == act_mklg ? gpos_mark2ligature :
		ot_undef;
    if ( otl->lookup_type == ot_undef )
	IError("Unknown AnchorClass type");
    otl->next = sf->sf.gpos_lookups;
    sf->sf.gpos_lookups = otl;
    otl->lookup_flags = ac->flags;
    otl->features = FeaturesFromTagSli(ac->feature_tag,ac->script_lang_index,sf);
    /* We will set the lookup_index after we've ordered the list */
    /* We will set the lookup_name after we've assigned the index */
    /* We will add one subtable soon */
return( otl );
}