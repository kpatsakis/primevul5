static void FPSTReplaceTagsWithLookups(FPST *fpst,SplineFont1 *sf) {
    int i,j,k;

    if ( fpst->type == pst_reversesub )
return;
    for ( i=0; i<fpst->rule_cnt; ++i ) {
	for ( j=0; j<fpst->rules[i].lookup_cnt; ++j ) {
	    OTLookup *otl = FindNestedLookupByTag(sf,(uint32) (intpt) (fpst->rules[i].lookups[j].lookup) );
	    if ( otl!=NULL )
		fpst->rules[i].lookups[j].lookup = otl;
	    else {
		for ( k=j+1; k<fpst->rules[i].lookup_cnt; ++k )
		    fpst->rules[i].lookups[k-1] = fpst->rules[i].lookups[k];
		--fpst->rules[i].lookup_cnt;
	    }
	}
    }
}