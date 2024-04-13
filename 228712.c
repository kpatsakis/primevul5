static SplineChar *SCDuplicate(SplineChar *sc) {
    SplineChar *matched = sc;

    if ( sc==NULL || sc->parent==NULL || sc->parent->cidmaster!=NULL )
return( sc );		/* Can't do this in CID keyed fonts */

    if ( sc->layer_cnt!=2 )
return( sc );

    while ( sc->layers[ly_fore].refs!=NULL &&
	    sc->layers[ly_fore].refs->sc!=NULL &&	/* Can happen if we are called during font loading before references are fixed up */
	    sc->layers[ly_fore].refs->next==NULL &&
	    sc->layers[ly_fore].refs->transform[0]==1 && sc->layers[ly_fore].refs->transform[1]==0 &&
	    sc->layers[ly_fore].refs->transform[2]==0 && sc->layers[ly_fore].refs->transform[3]==1 &&
	    sc->layers[ly_fore].refs->transform[4]==0 && sc->layers[ly_fore].refs->transform[5]==0 ) {
	char *basename = sc->layers[ly_fore].refs->sc->name;
	if ( strcmp(sc->name,basename)!=0 )
    break;
	matched = sc->layers[ly_fore].refs->sc;
	sc = sc->layers[ly_fore].refs->sc;
    }
return( matched );
}