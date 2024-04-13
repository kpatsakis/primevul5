static void SFDFixupUndoRefs(SplineFont *sf) {
    int i=0;
    Undoes *undo = 0;

    for ( i=0; i<sf->glyphcnt; ++i ) {
	if ( sf->glyphs[i]!=NULL ) {
	    SplineChar *sc = sf->glyphs[i];
	    int layer = 0;
	    for ( layer=0; layer<sc->layer_cnt; ++layer ) {
		if( sc->layers[layer].undoes ) {
		    undo = sc->layers[layer].undoes;
		    SFDFixupUndoRefsUndoList( sf, undo );
		}
		if( sc->layers[layer].redoes ) {
		    undo = sc->layers[layer].redoes;
		    SFDFixupUndoRefsUndoList( sf, undo );
		}
	    }
	}
    }


}