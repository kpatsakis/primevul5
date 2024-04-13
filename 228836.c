static int SFD_Dump( FILE *sfd, SplineFont *sf, EncMap *map, EncMap *normal,
		     int todir, char *dirname)
{
    int i, realcnt;
    BDFFont *bdf;
    int *newgids = NULL;
    int err = false;

    if ( normal!=NULL )
	map = normal;

    SFD_DumpSplineFontMetadata( sfd, sf ); //, map, normal, todir, dirname );

    if ( sf->MATH!=NULL ) {
	struct MATH *math = sf->MATH;
	for ( i=0; math_constants_descriptor[i].script_name!=NULL; ++i ) {
	    fprintf( sfd, "MATH:%s: %d", math_constants_descriptor[i].script_name,
		    *((int16 *) (((char *) (math)) + math_constants_descriptor[i].offset)) );
	    if ( math_constants_descriptor[i].devtab_offset>=0 ) {
		DeviceTable **devtab = (DeviceTable **) (((char *) (math)) + math_constants_descriptor[i].devtab_offset );
		putc(' ',sfd);
		SFDDumpDeviceTable(sfd,*devtab);
	    }
	    putc('\n',sfd);
	}
    }
    if ( sf->python_persistent!=NULL )
	SFDPickleMe(sfd,sf->python_persistent, sf->python_persistent_has_lists);
    if ( sf->subfontcnt!=0 ) {
	/* CID fonts have no encodings, they have registry info instead */
	fprintf(sfd, "Registry: %s\n", sf->cidregistry );
	fprintf(sfd, "Ordering: %s\n", sf->ordering );
	fprintf(sfd, "Supplement: %d\n", sf->supplement );
	fprintf(sfd, "CIDVersion: %g\n", (double) sf->cidversion );	/* This is a number whereas "version" is a string */
    } else
	SFDDumpEncoding(sfd,map->enc,"Encoding");
    if ( normal!=NULL )
	fprintf(sfd, "Compacted: 1\n" );
    fprintf( sfd, "UnicodeInterp: %s\n", unicode_interp_names[sf->uni_interp]);
    fprintf( sfd, "NameList: %s\n", sf->for_new_glyphs->title );

    if ( map->remap!=NULL ) {
	struct remap *remap;
	int n;
	for ( n=0,remap = map->remap; remap->infont!=-1; ++n, ++remap );
	fprintf( sfd, "RemapN: %d\n", n );
	for ( remap = map->remap; remap->infont!=-1; ++remap )
	    fprintf(sfd, "Remap: %x %x %d\n", (int) remap->firstenc, (int) remap->lastenc, (int) remap->infont );
    }
    if ( sf->display_size!=0 )
	fprintf( sfd, "DisplaySize: %d\n", sf->display_size );
    if ( sf->display_layer!=ly_fore )
	fprintf( sfd, "DisplayLayer: %d\n", sf->display_layer );
    fprintf( sfd, "AntiAlias: %d\n", sf->display_antialias );
    fprintf( sfd, "FitToEm: %d\n", sf->display_bbsized );
    if ( sf->extrema_bound!=0 )
	fprintf( sfd, "ExtremaBound: %d\n", sf->extrema_bound );
    if ( sf->width_separation!=0 )
	fprintf( sfd, "WidthSeparation: %d\n", sf->width_separation );
    {
	int rc, cc, te;
	if ( (te = FVWinInfo(sf->fv,&cc,&rc))!= -1 )
	    fprintf( sfd, "WinInfo: %d %d %d\n", te, cc, rc );
	else if ( sf->top_enc!=-1 )
	    fprintf( sfd, "WinInfo: %d %d %d\n", sf->top_enc, sf->desired_col_cnt,
		sf->desired_row_cnt);
    }
    if ( sf->onlybitmaps!=0 )
	fprintf( sfd, "OnlyBitmaps: %d\n", sf->onlybitmaps );
    if ( sf->private!=NULL )
	SFDDumpPrivate(sfd,sf->private);
#if HANYANG
    if ( sf->rules!=NULL )
	SFDDumpCompositionRules(sfd,sf->rules);
#endif
    if ( sf->grid.splines!=NULL ) {
	if ( sf->grid.order2 )
	    fprintf(sfd, "GridOrder2: %d\n", sf->grid.order2 );
	fprintf(sfd, "Grid\n" );
	SFDDumpSplineSet(sfd,sf->grid.splines,sf->grid.order2);
    }
    if ( sf->texdata.type!=tex_unset ) {
	fprintf(sfd, "TeXData: %d %d", (int) sf->texdata.type, (int) ((sf->design_size<<19)+2)/5 );
	for ( i=0; i<22; ++i )
	    fprintf(sfd, " %d", (int) sf->texdata.params[i]);
	putc('\n',sfd);
    }
    if ( sf->anchor!=NULL ) {
	AnchorClass *an;
	fprintf(sfd, "AnchorClass2:");
	for ( an=sf->anchor; an!=NULL; an=an->next ) {
	    putc(' ',sfd);
	    SFDDumpUTF7Str(sfd,an->name);
            if ( an->subtable!=NULL ) {
	        putc(' ',sfd);
	        SFDDumpUTF7Str(sfd,an->subtable->subtable_name);
            }
            else
                fprintf(sfd, "\"\" ");
	}
	putc('\n',sfd);
    }

    if ( sf->subfontcnt!=0 ) {
	if ( todir ) {
	    for ( i=0; i<sf->subfontcnt; ++i ) {
		char *subfont = malloc(strlen(dirname)+1+strlen(sf->subfonts[i]->fontname)+20);
		char *fontprops;
		FILE *ssfd;
		sprintf( subfont,"%s/%s" SUBFONT_EXT, dirname, sf->subfonts[i]->fontname );
		GFileMkDir(subfont, 0755);
		fontprops = malloc(strlen(subfont)+strlen("/" FONT_PROPS)+1);
		strcpy(fontprops,subfont); strcat(fontprops,"/" FONT_PROPS);
		ssfd = fopen( fontprops,"w");
		if ( ssfd!=NULL ) {
		    err |= SFD_Dump(ssfd,sf->subfonts[i],map,NULL,todir,subfont);
		    if ( ferror(ssfd) ) err = true;
		    if ( fclose(ssfd)) err = true;
		} else
		    err = true;
		free(fontprops);
		free(subfont);
	    }
	} else {
	    int max;
	    for ( i=max=0; i<sf->subfontcnt; ++i )
		if ( max<sf->subfonts[i]->glyphcnt )
		    max = sf->subfonts[i]->glyphcnt;
	    fprintf(sfd, "BeginSubFonts: %d %d\n", sf->subfontcnt, max );
	    for ( i=0; i<sf->subfontcnt; ++i )
		SFD_Dump(sfd,sf->subfonts[i],map,NULL,false, NULL);
	    fprintf(sfd, "EndSubFonts\n" );
	}
    } else {
	int enccount = map->enccount;
	if ( sf->cidmaster!=NULL ) {
	    realcnt = -1;
	    enccount = sf->glyphcnt;
	} else {
	    realcnt = 0;
	    for ( i=0; i<sf->glyphcnt; ++i ) if ( !SFDOmit(sf->glyphs[i]) )
		++realcnt;
	    if ( realcnt!=sf->glyphcnt ) {
		newgids = malloc(sf->glyphcnt*sizeof(int));
		realcnt = 0;
		for ( i=0; i<sf->glyphcnt; ++i )
		    if ( SFDOmit(sf->glyphs[i]) )
			newgids[i] = -1;
		    else
			newgids[i] = realcnt++;
	    }
	}
	if ( !todir )
	    fprintf(sfd, "BeginChars: %d %d\n",
	        enccount<map->enc->char_cnt? map->enc->char_cnt : enccount,
	        realcnt );
	for ( i=0; i<sf->glyphcnt; ++i ) {
	    if ( !SFDOmit(sf->glyphs[i]) ) {
		if ( !todir )
		SFDDumpChar(sfd,sf->glyphs[i],map,newgids,todir,1);
		else {
		    char *glyphfile = malloc(strlen(dirname)+2*strlen(sf->glyphs[i]->name)+20);
		    FILE *gsfd;
		    appendnames(glyphfile,dirname,"/",sf->glyphs[i]->name,GLYPH_EXT );
		    gsfd = fopen(glyphfile,"w");
		    if ( gsfd!=NULL ) {
			SFDDumpChar(gsfd,sf->glyphs[i],map,newgids,todir,1);
			if ( ferror(gsfd)) err = true;
			if ( fclose(gsfd)) err = true;
		    } else
			err = true;
		    free(glyphfile);
		}
	    }
	    ff_progress_next();
	}
	if ( !todir )
	    fprintf(sfd, "EndChars\n" );
    }

    if ( sf->bitmaps!=NULL )
	ff_progress_change_line2(_("Saving Bitmaps"));
    for ( bdf = sf->bitmaps; bdf!=NULL; bdf=bdf->next ) {
	if ( todir ) {
	    char *strike = malloc(strlen(dirname)+1+20+20);
	    char *strikeprops;
	    FILE *ssfd;
	    sprintf( strike,"%s/%d" STRIKE_EXT, dirname, bdf->pixelsize );
	    GFileMkDir(strike, 0755);
	    strikeprops = malloc(strlen(strike)+strlen("/" STRIKE_PROPS)+1);
	    strcpy(strikeprops,strike); strcat(strikeprops,"/" STRIKE_PROPS);
	    ssfd = fopen( strikeprops,"w");
	    if ( ssfd!=NULL ) {
		err |= SFDDumpBitmapFont(ssfd,bdf,map,newgids,todir,strike);
		if ( ferror(ssfd) ) err = true;
		if ( fclose(ssfd)) err = true;
	    } else
		err = true;
	    free(strikeprops);
	    free(strike);
	} else
	    SFDDumpBitmapFont(sfd,bdf,map,newgids,todir,dirname);
    }
    fprintf(sfd, sf->cidmaster==NULL?"EndSplineFont\n":"EndSubSplineFont\n" );
    free(newgids);
return( err );
}