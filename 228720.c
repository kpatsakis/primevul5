static void ACHasBaseLig(SplineFont1 *sf,AnchorClass1 *ac) {
    int gid,k;
    SplineFont1 *subsf;
    SplineChar *sc;
    AnchorPoint *ap;

    ac->has_bases = ac->has_ligatures = false;
    if ( ac->ac.type==act_mkmk || ac->ac.type==act_curs )
return;
    k=0;
    do {
	subsf = sf->sf.subfontcnt==0 ? sf : (SplineFont1 *) (sf->sf.subfonts[k]);
	for ( gid=0; gid<subsf->sf.glyphcnt; ++gid ) if ( (sc=subsf->sf.glyphs[gid])!=NULL ) {
	    for ( ap=sc->anchor; ap!=NULL; ap=ap->next ) {
		if ( ap->anchor!=(AnchorClass *) ac )
	    continue;
		if ( ap->type==at_basechar ) {
		    ac->has_bases = true;
		    if ( ac->has_ligatures )
return;
		} else if ( ap->type==at_baselig ) {
		    ac->has_ligatures = true;
		    if ( ac->has_bases )
return;
		}
	    }
	}
	++k;
    } while ( k<sf->sf.subfontcnt );
}