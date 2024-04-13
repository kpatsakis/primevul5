static void SFDFixupRef(SplineChar *sc,RefChar *ref,int layer) {
    RefChar *rf;
    int ly;

    if ( sc->parent->multilayer ) {
	for ( ly=ly_fore; ly<ref->sc->layer_cnt; ++ly ) {
	    for ( rf = ref->sc->layers[ly].refs; rf!=NULL; rf=rf->next ) {
		if ( rf->sc==sc ) {	/* Huh? */
		    ref->sc->layers[ly].refs = NULL;
	    break;
		}
		if ( rf->layers[0].splines==NULL )
		    SFDFixupRef(ref->sc,rf,layer);
	    }
	}
    } else {
	for ( rf = ref->sc->layers[layer].refs; rf!=NULL; rf=rf->next ) {
	    if ( rf->sc==sc ) {	/* Huh? */
		ref->sc->layers[layer].refs = NULL;
	break;
	    }
	    if ( rf->layers[0].splines==NULL )
		SFDFixupRef(ref->sc,rf,layer);
	}
    }
    SCReinstanciateRefChar(sc,ref,layer);
    SCMakeDependent(sc,ref->sc);
}