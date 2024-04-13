static int ModSF(FILE *asfd,SplineFont *sf) {
    Encoding *newmap;
    int cnt;
    int multilayer=0;
    char tok[200];
    int i,k;
    SplineChar *sc;
    SplineFont *ssf;
    SplineFont temp;
    int layercnt;

    memset(&temp,0,sizeof(temp));
    temp.layers = sf->layers;
    temp.layer_cnt = sf->layer_cnt;
    temp.layers[ly_back].order2 = sf->layers[ly_back].order2;
    temp.layers[ly_fore].order2 = sf->layers[ly_fore].order2;
    temp.ascent = sf->ascent; temp.descent = sf->descent;
    temp.multilayer = sf->multilayer;
    temp.gpos_lookups = sf->gpos_lookups;
    temp.gsub_lookups = sf->gsub_lookups;
    temp.anchor = sf->anchor;
    temp.sfd_version = 2;

    if ( getname(asfd,tok)!=1 || strcmp(tok,"Encoding:")!=0 )
return(false);
    newmap = SFDGetEncoding(asfd,tok);
    if ( getname(asfd,tok)!=1 )
return( false );
    if ( strcmp(tok,"UnicodeInterp:")==0 ) {
	sf->uni_interp = SFDGetUniInterp(asfd,tok,sf);
	if ( getname(asfd,tok)!=1 )
return( false );
    }
    if ( sf->map!=NULL && sf->map->enc!=newmap ) {
	EncMap *map = EncMapFromEncoding(sf,newmap);
	EncMapFree(sf->map);
	sf->map = map;
    }
    temp.map = sf->map;
    if ( strcmp(tok,"LayerCount:")==0 ) {
	getint(asfd,&layercnt);
	if ( layercnt>sf->layer_cnt ) {
	    sf->layers = realloc(sf->layers,layercnt*sizeof(LayerInfo));
	    memset(sf->layers+sf->layer_cnt,0,(layercnt-sf->layer_cnt)*sizeof(LayerInfo));
	}
	sf->layer_cnt = layercnt;
	if ( getname(asfd,tok)!=1 )
return( false );
    }
    while ( strcmp(tok,"Layer:")==0 ) {
	int layer, o2;
	getint(asfd,&layer);
	getint(asfd,&o2);
	if ( layer<sf->layer_cnt ) {
	    sf->layers[layer].order2 = o2;
		if (sf->layers[layer].name)
		    free(sf->layers[layer].name);
	    sf->layers[layer].name = SFDReadUTF7Str(asfd);
	}
	if ( getname(asfd,tok)!=1 )
return( false );
    }
    if ( strcmp(tok,"MultiLayer:")==0 ) {
	getint(asfd,&multilayer);
	if ( getname(asfd,tok)!=1 )
return( false );
    }
    if ( multilayer!=sf->multilayer ) {
	if ( !multilayer )
	    SFSplinesFromLayers(sf,false);
	sf->multilayer = multilayer;
	/* SFLayerChange(sf);*/		/* Shouldn't have any open windows, should not be needed */
    }
    if ( strcmp(tok,"BeginChars:")!=0 )
return(false);
    SFRemoveDependencies(sf);

    getint(asfd,&cnt);
    if ( cnt>sf->glyphcnt ) {
	sf->glyphs = realloc(sf->glyphs,cnt*sizeof(SplineChar *));
	for ( i=sf->glyphcnt; i<cnt; ++i )
	    sf->glyphs[i] = NULL;
	sf->glyphcnt = sf->glyphmax = cnt;
    }
    while ( (sc = SFDGetChar(asfd,&temp,true))!=NULL ) {
	ssf = sf;
	for ( k=0; k<sf->subfontcnt; ++k ) {
	    if ( sc->orig_pos<sf->subfonts[k]->glyphcnt ) {
		ssf = sf->subfonts[k];
		if ( SCWorthOutputting(ssf->glyphs[sc->orig_pos]))
	break;
	    }
	}
	if ( sc->orig_pos<ssf->glyphcnt ) {
	    if ( ssf->glyphs[sc->orig_pos]!=NULL )
		SplineCharFree(ssf->glyphs[sc->orig_pos]);
	    ssf->glyphs[sc->orig_pos] = sc;
	    sc->parent = ssf;
	    sc->changed = true;
	}
    }
    sf->changed = true;
    SFDFixupRefs(sf);
return(true);
}