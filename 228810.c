static void SFRemoveDependencies(SplineFont *sf) {
    int i;
    struct splinecharlist *dlist, *dnext;
    KernPair *kp;

    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	for ( dlist = sf->glyphs[i]->dependents; dlist!=NULL; dlist = dnext ) {
	    dnext = dlist->next;
	    chunkfree(dlist,sizeof(*dlist));
	}
	sf->glyphs[i]->dependents = NULL;
	for ( kp=sf->glyphs[i]->kerns; kp!=NULL; kp=kp->next ) {
	    kp->sc = (SplineChar *) (intpt) (kp->sc->orig_pos);
	    kp->kcid = true;		/* flag */
	}
	for ( kp=sf->glyphs[i]->vkerns; kp!=NULL; kp=kp->next ) {
	    kp->sc = (SplineChar *) (intpt) (kp->sc->orig_pos);
	    kp->kcid = true;
	}
    }
}