static SplineFont *SFD_FigureDirType(SplineFont *sf,char *tok, char *dirname,
	Encoding *enc, struct remap *remap,int had_layer_cnt) {
    /* In a sfdir a directory will either contain glyph files */
    /*                                            subfont dirs */
    /*                                            instance dirs */
    /* (or bitmap files, but we don't care about them here */
    /* It will not contain some glyph and some subfont nor instance files */
    int gc=0, sc=0, ic=0, bc=0;
    DIR *dir;
    struct dirent *ent;
    char *name, *props, *pt;

    dir = opendir(dirname);
    if ( dir==NULL )
return( sf );
    sf->save_to_dir = true;
    while ( (ent=readdir(dir))!=NULL ) {
	pt = strrchr(ent->d_name,EXT_CHAR);
	if ( pt==NULL )
	    /* Nothing interesting */;
	else if ( strcmp(pt,GLYPH_EXT)==0 )
	    ++gc;
	else if ( strcmp(pt,SUBFONT_EXT)==0 )
	    ++sc;
	else if ( strcmp(pt,INSTANCE_EXT)==0 )
	    ++ic;
	else if ( strcmp(pt,STRIKE_EXT)==0 )
	    ++bc;
    }
    rewinddir(dir);
    name = malloc(strlen(dirname)+NAME_MAX+3);
    props = malloc(strlen(dirname)+2*NAME_MAX+4);
    if ( gc!=0 ) {
	sf->glyphcnt = 0;
	sf->glyphmax = gc;
	sf->glyphs = calloc(gc,sizeof(SplineChar *));
	ff_progress_change_total(gc);
	if ( sf->cidmaster!=NULL ) {
	    sf->map = sf->cidmaster->map;
	} else {
	    sf->map = EncMapNew(enc->char_cnt>gc?enc->char_cnt:gc,gc,enc);
	    sf->map->remap = remap;
	}
	SFDSizeMap(sf->map,sf->glyphcnt,enc->char_cnt>gc?enc->char_cnt:gc);

	while ( (ent=readdir(dir))!=NULL ) {
	    pt = strrchr(ent->d_name,EXT_CHAR);
	    if ( pt==NULL )
		/* Nothing interesting */;
	    else if ( strcmp(pt,GLYPH_EXT)==0 ) {
		FILE *gsfd;
		sprintf(name,"%s/%s", dirname, ent->d_name);
		gsfd = fopen(name,"r");
		if ( gsfd!=NULL ) {
		    SFDGetChar(gsfd,sf,had_layer_cnt);
		    ff_progress_next();
		    fclose(gsfd);
		}
	    }
	}
	ff_progress_next_stage();
    } else if ( sc!=0 ) {
	int i=0;
	sf->subfontcnt = sc;
	sf->subfonts = calloc(sf->subfontcnt,sizeof(SplineFont *));
	sf->map = EncMap1to1(1000);
	ff_progress_change_stages(2*sc);

	while ( (ent=readdir(dir))!=NULL ) {
	    pt = strrchr(ent->d_name,EXT_CHAR);
	    if ( pt==NULL )
		/* Nothing interesting */;
	    else if ( strcmp(pt,SUBFONT_EXT)==0 && i<sc ) {
		FILE *ssfd;
		sprintf(name,"%s/%s", dirname, ent->d_name);
		sprintf(props,"%s/" FONT_PROPS, name);
		ssfd = fopen(props,"r");
		if ( ssfd!=NULL ) {
		    if ( i!=0 )
			ff_progress_next_stage();
		    sf->subfonts[i++] = SFD_GetFont(ssfd,sf,tok,true,name,sf->sfd_version);
		    fclose(ssfd);
		}
	    }
	}
    } else if ( ic!=0 ) {
	MMSet *mm = sf->mm;
	int ipos, i=0;

	MMInferStuff(sf->mm);
	ff_progress_change_stages(2*(mm->instance_count+1));
	while ( (ent=readdir(dir))!=NULL ) {
	    pt = strrchr(ent->d_name,EXT_CHAR);
	    if ( pt==NULL )
		/* Nothing interesting */;
	    else if ( strcmp(pt,INSTANCE_EXT)==0 && sscanf( ent->d_name, "mm%d", &ipos)==1 ) {
		FILE *ssfd;
		if ( i!=0 )
		    ff_progress_next_stage();
		sprintf(name,"%s/%s", dirname, ent->d_name);
		sprintf(props,"%s/" FONT_PROPS, name);
		ssfd = fopen(props,"r");
		if ( ssfd!=NULL ) {
		    SplineFont *mmsf;
		    mmsf = SFD_GetFont(ssfd,NULL,tok,true,name,sf->sfd_version);
		    if ( ipos!=0 ) {
			EncMapFree(mmsf->map);
			mmsf->map=NULL;
		    }
		    mmsf->mm = mm;
		    if ( ipos == 0 )
			mm->normal = mmsf;
		    else
			mm->instances[ipos-1] = mmsf;
		    fclose(ssfd);
		}
	    }
	}
	ff_progress_next_stage();
	sf->mm = NULL;
	SplineFontFree(sf);
	sf = mm->normal;
	if ( sf->map->enc!=&custom ) {
	    EncMap *map;
	    MMMatchGlyphs(mm);		/* sfd files from before the encoding change can have mismatched orig pos */
	    map = EncMapFromEncoding(sf,sf->map->enc);
	    EncMapFree(sf->map);
	    sf->map = map;
	}
    }

    if ( bc!=0 ) {
	rewinddir(dir);
	while ( (ent=readdir(dir))!=NULL ) {
	    pt = strrchr(ent->d_name,EXT_CHAR);
	    if ( pt==NULL )
		/* Nothing interesting */;
	    else if ( strcmp(pt,STRIKE_EXT)==0 ) {
		FILE *ssfd;
		sprintf(name,"%s/%s", dirname, ent->d_name);
		sprintf(props,"%s/" STRIKE_PROPS, name);
		ssfd = fopen(props,"r");
		if ( ssfd!=NULL ) {
		    if ( getname(ssfd,tok)==1 && strcmp(tok,"BitmapFont:")==0 )
			SFDGetBitmapFont(ssfd,sf,true,name);
		    fclose(ssfd);
		}
	    }
	}
	SFOrderBitmapList(sf);
    }
    closedir(dir);
    free(name);
    free(props);
return( sf );
}