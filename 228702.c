static SplineFont *SFD_Read(char *filename,FILE *sfd, int fromdir) {
    SplineFont *sf=NULL;
    char tok[2000];
    double version;

    if ( sfd==NULL ) {
	if ( fromdir ) {
	    snprintf(tok,sizeof(tok),"%s/" FONT_PROPS, filename );
	    sfd = fopen(tok,"r");
	} else
	    sfd = fopen(filename,"r");
    }
    if ( sfd==NULL )
return( NULL );
    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.
    ff_progress_change_stages(2);
    if ( (version = SFDStartsCorrectly(sfd,tok))!=-1 )
	sf = SFD_GetFont(sfd,NULL,tok,fromdir,filename,version);
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
    if ( sf!=NULL ) {
	sf->filename = copy(filename);
	if ( sf->mm!=NULL ) {
	    int i;
	    for ( i=0; i<sf->mm->instance_count; ++i )
		sf->mm->instances[i]->filename = copy(filename);
	} else if ( !sf->onlybitmaps ) {
/* Jonathyn Bet'nct points out that once you edit in an outline window, even */
/*  if by mistake, your onlybitmaps status is gone for good */
/* Regenerate it if the font has no splines, refs, etc. */
	    int i;
	    SplineChar *sc;
	    for ( i=sf->glyphcnt-1; i>=0; --i )
		if ( (sc = sf->glyphs[i])!=NULL &&
			(sc->layer_cnt!=2 ||
			 sc->layers[ly_fore].splines!=NULL ||
			 sc->layers[ly_fore].refs!=NULL ))
	     break;
	     if ( i==-1 )
		 sf->onlybitmaps = true;
	}
    }
    fclose(sfd);
return( sf );
}