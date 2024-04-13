static OTLookup *CreateLookup(SplineFont1 *sf,uint32 tag, int sli,
	int flags,enum possub_type type) {
    OTLookup *otl = chunkalloc(sizeof(OTLookup));

    otl->lookup_type =
	    type == pst_position ? gpos_single :
	    type == pst_pair ? gpos_pair :
	    type == pst_contextpos ? gpos_context :
	    type == pst_chainpos ? gpos_contextchain :
	    type == pst_substitution ? gsub_single :
	    type == pst_alternate ? gsub_alternate :
	    type == pst_multiple ? gsub_multiple :
	    type == pst_ligature ? gsub_ligature :
	    type == pst_contextsub ? gsub_context :
	    type == pst_chainsub ? gsub_contextchain :
		ot_undef;
    if ( otl->lookup_type == ot_undef )
	IError("Unknown lookup type");
    if ( otl->lookup_type<gpos_single ) {
	otl->next = sf->sf.gsub_lookups;
	sf->sf.gsub_lookups = otl;
    } else {
	otl->next = sf->sf.gpos_lookups;
	sf->sf.gpos_lookups = otl;
    }
    otl->lookup_flags = flags;
    otl->features = FeaturesFromTagSli(tag,sli,sf);
    /* We will set the lookup_index after we've ordered the list */
    /* We will set the lookup_name after we've assigned the index */
    /* We will add subtables as we need them */
return( otl );
}