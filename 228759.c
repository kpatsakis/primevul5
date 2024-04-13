void SFAutoSave(SplineFont *sf,EncMap *map) {
    int i, k, max;
    FILE *asfd;
    SplineFont *ssf;

    if ( no_windowing_ui )		/* No autosaves when just scripting */
return;

    if ( sf->cidmaster!=NULL ) sf=sf->cidmaster;
    asfd = fopen(sf->autosavename,"w");
    if ( asfd==NULL )
return;

    max = sf->glyphcnt;
    for ( i=0; i<sf->subfontcnt; ++i )
	if ( sf->subfonts[i]->glyphcnt>max ) max = sf->subfonts[i]->glyphcnt;

    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.
    if ( !sf->new && sf->origname!=NULL )	/* might be a new file */
	fprintf( asfd, "Base: %s%s\n", sf->origname,
		sf->compression==0?"":compressors[sf->compression-1].ext );
    fprintf( asfd, "Encoding: %s\n", map->enc->enc_name );
    fprintf( asfd, "UnicodeInterp: %s\n", unicode_interp_names[sf->uni_interp]);
    fprintf( asfd, "LayerCount: %d\n", sf->layer_cnt );
    for ( i=0; i<sf->layer_cnt; ++i ) {
	fprintf( asfd, "Layer: %d %d ", i, sf->layers[i].order2 );
	SFDDumpUTF7Str(asfd,sf->layers[i].name);
	putc('\n',asfd);
    }
    if ( sf->multilayer )
	fprintf( asfd, "MultiLayer: %d\n", sf->multilayer );
    fprintf( asfd, "BeginChars: %d\n", max );
    for ( i=0; i<max; ++i ) {
	ssf = sf;
	for ( k=0; k<sf->subfontcnt; ++k ) {
	    if ( i<sf->subfonts[k]->glyphcnt ) {
		ssf = sf->subfonts[k];
		if ( SCWorthOutputting(ssf->glyphs[i]))
	break;
	    }
	}
	if ( ssf->glyphs[i]!=NULL && ssf->glyphs[i]->changed )
	    SFDDumpChar( asfd,ssf->glyphs[i],map,NULL,false,1);
    }
    fprintf( asfd, "EndChars\n" );
    fprintf( asfd, "EndSplineFont\n" );
    fclose(asfd);
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
    sf->changed_since_autosave = false;
}