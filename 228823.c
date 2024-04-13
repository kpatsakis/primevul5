static int SFDOmit(SplineChar *sc) {
    int layer;
    BDFFont *bdf;

    if ( sc==NULL )
return( true );
    if ( sc->comment!=NULL || sc->color!=COLOR_DEFAULT )
return( false );
    for ( layer = ly_back; layer<sc->layer_cnt; ++layer ) {
	if ( sc->layers[layer].splines!=NULL ||
		sc->layers[layer].refs!=NULL ||
		sc->layers[layer].images!=NULL )
return( false );
    }
    if ( sc->parent->onlybitmaps ) {
	for ( bdf = sc->parent->bitmaps; bdf!=NULL; bdf=bdf->next ) {
	    if ( sc->orig_pos<bdf->glyphcnt && bdf->glyphs[sc->orig_pos]!=NULL )
return( false );
	}
    }
    if ( !sc->widthset )
return(true);

return( false );
}