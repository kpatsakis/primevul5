void SFClearAutoSave(SplineFont *sf) {
    int i;
    SplineFont *ssf;

    if ( sf->cidmaster!=NULL ) sf = sf->cidmaster;
    sf->changed_since_autosave = false;
    for ( i=0; i<sf->subfontcnt; ++i ) {
	ssf = sf->subfonts[i];
	ssf->changed_since_autosave = false;
	if ( ssf->autosavename!=NULL ) {
	    unlink( ssf->autosavename );
	    free( ssf->autosavename );
	    ssf->autosavename = NULL;
	}
    }
    if ( sf->autosavename==NULL )
return;
    unlink(sf->autosavename);
    free(sf->autosavename);
    sf->autosavename = NULL;
}