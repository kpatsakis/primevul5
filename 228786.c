static void SFD_DoAltUnis(SplineFont *sf) {
    int i;
    struct altuni *alt;
    SplineChar *sc;

    for ( i=0; i<sf->glyphcnt; ++i ) if ( (sc = sf->glyphs[i])!=NULL ) {
	for ( alt = sc->altuni; alt!=NULL; alt = alt->next ) {
	    if ( alt->vs==-1 && alt->fid==0 ) {
		int enc = EncFromUni(alt->unienc,sf->map->enc);
		if ( enc!=-1 )
		    SFDSetEncMap(sf,sc->orig_pos,enc);
	    }
	}
    }
}