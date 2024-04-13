static OTLookup *FindNestedLookupByTag(SplineFont1 *sf,uint32 tag) {
    int isgpos;
    OTLookup *otl;

    for ( isgpos=0; isgpos<2; ++isgpos ) {
	for ( otl = isgpos ? sf->sf.gpos_lookups : sf->sf.gsub_lookups; otl!=NULL; otl=otl->next ) {
	    if ( otl->features!=NULL && otl->features->scripts==NULL &&
		    otl->features->featuretag == tag )
return( otl );
	}
    }
return( NULL );
}