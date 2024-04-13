SplineChar *SFDReadOneChar(SplineFont *cur_sf,const char *name) {
    FILE *sfd;
    SplineChar *sc=NULL;
    char oldloc[25], tok[2000];
    uint32 pos;
    SplineFont sf;
    LayerInfo layers[2];
    double version;
    int had_layer_cnt=false;
    int chars_seen = false;

    if ( cur_sf->save_to_dir ) {
	snprintf(tok,sizeof(tok),"%s/" FONT_PROPS,cur_sf->filename);
	sfd = fopen(tok,"r");
    } else
	sfd = fopen(cur_sf->filename,"r");
    if ( sfd==NULL )
return( NULL );
    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.

    memset(&sf,0,sizeof(sf));
    memset(&layers,0,sizeof(layers));
    sf.layer_cnt = 2;
    sf.layers = layers;
    sf.ascent = 800; sf.descent = 200;
    if ( cur_sf->cidmaster ) cur_sf = cur_sf->cidmaster;
    if ( (version = SFDStartsCorrectly(sfd,tok))>=2 ) {
	sf.sfd_version = version;
	sf.gpos_lookups = cur_sf->gpos_lookups;
	sf.gsub_lookups = cur_sf->gsub_lookups;
	sf.anchor = cur_sf->anchor;
	pos = ftell(sfd);
	while ( getname(sfd,tok)!=-1 ) {
	    if ( strcmp(tok,"StartChar:")==0 ) {
		if ( getname(sfd,tok)==1 && strcmp(tok,name)==0 ) {
		    fseek(sfd,pos,SEEK_SET);
		    sc = SFDGetChar(sfd,&sf,had_layer_cnt);
	break;
		}
	    } else if ( strmatch(tok,"BeginChars:")==0 ) {
		chars_seen = true;
	    } else if ( chars_seen ) {
		/* Don't try to look for things in the file header any more */
		/* The "Layer" keyword has a different meaning in this context */
	    } else if ( strmatch(tok,"Order2:")==0 ) {
		int order2;
		getint(sfd,&order2);
		sf.grid.order2 = order2;
		sf.layers[ly_back].order2 = order2;
		sf.layers[ly_fore].order2 = order2;
	    } else if ( strmatch(tok,"LayerCount:")==0 ) {
		had_layer_cnt = true;
		getint(sfd,&sf.layer_cnt);
		if ( sf.layer_cnt>2 ) {
		    sf.layers = calloc(sf.layer_cnt,sizeof(LayerInfo));
		}
	    } else if ( strmatch(tok,"Layer:")==0 ) {
		int layer, o2;
		getint(sfd,&layer);
		getint(sfd,&o2);
		if ( layer<sf.layer_cnt )
		    sf.layers[layer].order2 = o2;
		free( SFDReadUTF7Str(sfd));
	    } else if ( strmatch(tok,"MultiLayer:")==0 ) {
		int ml;
		getint(sfd,&ml);
		sf.multilayer = ml;
	    } else if ( strmatch(tok,"StrokedFont:")==0 ) {
		int stk;
		getint(sfd,&stk);
		sf.strokedfont = stk;
	    } else if ( strmatch(tok,"Ascent:")==0 ) {
		getint(sfd,&sf.ascent);
	    } else if ( strmatch(tok,"Descent:")==0 ) {
		getint(sfd,&sf.descent);
	    } else if ( strmatch(tok,"InvalidEm:")==0 ) {
		getint(sfd,&sf.invalidem);
	    }
	    pos = ftell(sfd);
	}
    }
    fclose(sfd);
    if ( cur_sf->save_to_dir ) {
	if ( sc!=NULL ) IError("Read a glyph from font.props");
	/* Doesn't work for CID keyed, nor for mm */
	snprintf(tok,sizeof(tok),"%s/%s" GLYPH_EXT,cur_sf->filename,name);
	sfd = fopen(tok,"r");
	if ( sfd!=NULL ) {
	    sc = SFDGetChar(sfd,&sf,had_layer_cnt);
	    fclose(sfd);
	}
    }

    if ( sf.layers!=layers )
	free(sf.layers);
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
return( sc );
}