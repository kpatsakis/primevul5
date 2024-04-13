static SplineChar *SFDGetChar(FILE *sfd,SplineFont *sf, int had_sf_layer_cnt) {
    SplineChar *sc;
    char tok[2000], ch;
    RefChar *lastr=NULL, *ref;
    ImageList *lasti=NULL, *img;
    AnchorPoint *lastap = NULL;
    GuidelineSet *lastgl = NULL;
    int isliga = 0, ispos, issubs, ismult, islcar, ispair, temp, i;
    PST *last = NULL;
    uint32 script = 0;
    int current_layer = ly_fore;
    int multilayer = sf->multilayer;
    int had_old_dstems = false;
    SplineFont *sli_sf = sf->cidmaster ? sf->cidmaster : sf;
    struct altuni *altuni;
    int oldback = false;

    if ( getname(sfd,tok)!=1 )
return( NULL );
    if ( strcmp(tok,"StartChar:")!=0 )
return( NULL );
    while ( isspace(ch=nlgetc(sfd)));
    ungetc(ch,sfd);
    sc = SFSplineCharCreate(sf);
    if ( ch!='"' ) {
	if ( getname(sfd,tok)!=1 ) {
	    SplineCharFree(sc);
return( NULL );
	}
	sc->name = copy(tok);
    } else {
	sc->name = SFDReadUTF7Str(sfd);
	if ( sc->name==NULL ) {
	    SplineCharFree(sc);
return( NULL );
	}
    }
    sc->vwidth = sf->ascent+sf->descent;
    sc->parent = sf;
    while ( 1 ) {
	if ( getname(sfd,tok)!=1 ) {
	    SplineCharFree(sc);
return( NULL );
	}
	if ( strmatch(tok,"Encoding:")==0 ) {
	    int enc;
	    getint(sfd,&enc);
	    getint(sfd,&sc->unicodeenc);
	    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
	    ungetc(ch,sfd);
	    if ( ch!='\n' && ch!='\r' ) {
		getint(sfd,&sc->orig_pos);
		if ( sc->orig_pos==65535 )
		    sc->orig_pos = orig_pos++;
		    /* An old mark meaning: "I don't know" */
		if ( sc->orig_pos<sf->glyphcnt && sf->glyphs[sc->orig_pos]!=NULL )
		    sc->orig_pos = sf->glyphcnt;
		if ( sc->orig_pos>=sf->glyphcnt ) {
		    if ( sc->orig_pos>=sf->glyphmax )
			sf->glyphs = realloc(sf->glyphs,(sf->glyphmax = sc->orig_pos+10)*sizeof(SplineChar *));
		    memset(sf->glyphs+sf->glyphcnt,0,(sc->orig_pos+1-sf->glyphcnt)*sizeof(SplineChar *));
		    sf->glyphcnt = sc->orig_pos+1;
		}
		if ( sc->orig_pos+1 > orig_pos )
		    orig_pos = sc->orig_pos+1;
	    } else if ( sf->cidmaster!=NULL ) {		/* In cid fonts the orig_pos is just the cid */
		sc->orig_pos = enc;
	    } else {
		sc->orig_pos = orig_pos++;
	    }
	    SFDSetEncMap(sf,sc->orig_pos,enc);
	} else if ( strmatch(tok,"AltUni:")==0 ) {
	    int uni;
	    while ( getint(sfd,&uni)==1 ) {
		altuni = chunkalloc(sizeof(struct altuni));
		altuni->unienc = uni;
		altuni->vs = -1;
		altuni->fid = 0;
		altuni->next = sc->altuni;
		sc->altuni = altuni;
	    }
	} else if ( strmatch(tok,"AltUni2:")==0 ) {
	    uint32 uni[3];
	    while ( gethexints(sfd,uni,3) ) {
		altuni = chunkalloc(sizeof(struct altuni));
		altuni->unienc = uni[0];
		altuni->vs = uni[1];
		altuni->fid = uni[2];
		altuni->next = sc->altuni;
		sc->altuni = altuni;
	    }
	} else if ( strmatch(tok,"OldEncoding:")==0 ) {
	    int old_enc;		/* Obsolete info */
	    getint(sfd,&old_enc);
        } else if ( strmatch(tok,"Script:")==0 ) {
	    /* Obsolete. But still used for parsing obsolete ligature/subs tags */
            while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
            if ( ch=='\n' || ch=='\r' )
                script = 0;
            else {
		ungetc(ch,sfd);
		script = gettag(sfd);
            }
	} else if ( strmatch(tok,"GlifName:")==0 ) {
            while ( isspace(ch=nlgetc(sfd)));
            ungetc(ch,sfd);
            if ( ch!='"' ) {
              if ( getname(sfd,tok)!=1 ) {
                LogError(_("Invalid glif name.\n"));
              }
	      sc->glif_name = copy(tok);
            } else {
	      sc->glif_name = SFDReadUTF7Str(sfd);
	      if ( sc->glif_name==NULL ) {
                LogError(_("Invalid glif name.\n"));
	      }
            }
	} else if ( strmatch(tok,"Width:")==0 ) {
	    getsint(sfd,&sc->width);
	} else if ( strmatch(tok,"VWidth:")==0 ) {
	    getsint(sfd,&sc->vwidth);
	} else if ( strmatch(tok,"GlyphClass:")==0 ) {
	    getint(sfd,&temp);
	    sc->glyph_class = temp;
	} else if ( strmatch(tok,"UnlinkRmOvrlpSave:")==0 ) {
	    getint(sfd,&temp);
	    sc->unlink_rm_ovrlp_save_undo = temp;
	} else if ( strmatch(tok,"InSpiro:")==0 ) {
	    getint(sfd,&temp);
	    sc->inspiro = temp;
	} else if ( strmatch(tok,"LigCaretCntFixed:")==0 ) {
	    getint(sfd,&temp);
	    sc->lig_caret_cnt_fixed = temp;
	} else if ( strmatch(tok,"Flags:")==0 ) {
	    while ( isspace(ch=nlgetc(sfd)) && ch!='\n' && ch!='\r');
	    while ( ch!='\n' && ch!='\r' ) {
		if ( ch=='H' ) sc->changedsincelasthinted=true;
		else if ( ch=='M' ) sc->manualhints = true;
		else if ( ch=='W' ) sc->widthset = true;
		else if ( ch=='O' ) sc->wasopen = true;
		else if ( ch=='I' ) sc->instructions_out_of_date = true;
		ch = nlgetc(sfd);
	    }
	    if ( sf->multilayer || sf->onlybitmaps || sf->strokedfont || sc->layers[ly_fore].order2 )
		sc->changedsincelasthinted = false;
	} else if ( strmatch(tok,"TeX:")==0 ) {
	    getsint(sfd,&sc->tex_height);
	    getsint(sfd,&sc->tex_depth);
	    while ( isspace(ch=nlgetc(sfd)) && ch!='\n' && ch!='\r');
	    ungetc(ch,sfd);
	    if ( ch!='\n' && ch!='\r' ) {
		int16 old_tex;
		/* Used to store two extra values here */
		getsint(sfd,&old_tex);
		getsint(sfd,&old_tex);
		if ( sc->tex_height==0 && sc->tex_depth==0 )		/* Fixup old bug */
		    sc->tex_height = sc->tex_depth = TEX_UNDEF;
	    }
	} else if ( strmatch(tok,"ItalicCorrection:")==0 ) {
	    SFDParseMathValueRecord(sfd,&sc->italic_correction,&sc->italic_adjusts);
	} else if ( strmatch(tok,"TopAccentHorizontal:")==0 ) {
	    SFDParseMathValueRecord(sfd,&sc->top_accent_horiz,&sc->top_accent_adjusts);
	} else if ( strmatch(tok,"GlyphCompositionVerticalIC:")==0 ) {
	    if ( sc->vert_variants==NULL )
		sc->vert_variants = chunkalloc(sizeof(struct glyphvariants));
	    SFDParseMathValueRecord(sfd,&sc->vert_variants->italic_correction,&sc->vert_variants->italic_adjusts);
	} else if ( strmatch(tok,"GlyphCompositionHorizontalIC:")==0 ) {
	    if ( sc->horiz_variants==NULL )
		sc->horiz_variants = chunkalloc(sizeof(struct glyphvariants));
	    SFDParseMathValueRecord(sfd,&sc->horiz_variants->italic_correction,&sc->horiz_variants->italic_adjusts);
	} else if ( strmatch(tok,"IsExtendedShape:")==0 ) {
	    int temp;
	    getint(sfd,&temp);
	    sc->is_extended_shape = temp;
	} else if ( strmatch(tok,"GlyphVariantsVertical:")==0 ) {
	    if ( sc->vert_variants==NULL )
		sc->vert_variants = chunkalloc(sizeof(struct glyphvariants));
	    geteol(sfd,tok);
	    sc->vert_variants->variants = copy(tok);
	} else if ( strmatch(tok,"GlyphVariantsHorizontal:")==0 ) {
	    if ( sc->horiz_variants==NULL )
		sc->horiz_variants = chunkalloc(sizeof(struct glyphvariants));
	    geteol(sfd,tok);
	    sc->horiz_variants->variants = copy(tok);
	} else if ( strmatch(tok,"GlyphCompositionVertical:")==0 ) {
	    sc->vert_variants = SFDParseGlyphComposition(sfd, sc->vert_variants,tok);
	} else if ( strmatch(tok,"GlyphCompositionHorizontal:")==0 ) {
	    sc->horiz_variants = SFDParseGlyphComposition(sfd, sc->horiz_variants,tok);
	} else if ( strmatch(tok,"TopRightVertex:")==0 ) {
	    if ( sc->mathkern==NULL )
		sc->mathkern = chunkalloc(sizeof(struct mathkern));
	    SFDParseVertexKern(sfd, &sc->mathkern->top_right);
	} else if ( strmatch(tok,"TopLeftVertex:")==0 ) {
	    if ( sc->mathkern==NULL )
		sc->mathkern = chunkalloc(sizeof(struct mathkern));
	    SFDParseVertexKern(sfd, &sc->mathkern->top_left);
	} else if ( strmatch(tok,"BottomRightVertex:")==0 ) {
	    if ( sc->mathkern==NULL )
		sc->mathkern = chunkalloc(sizeof(struct mathkern));
	    SFDParseVertexKern(sfd, &sc->mathkern->bottom_right);
	} else if ( strmatch(tok,"BottomLeftVertex:")==0 ) {
	    if ( sc->mathkern==NULL )
		sc->mathkern = chunkalloc(sizeof(struct mathkern));
	    SFDParseVertexKern(sfd, &sc->mathkern->bottom_left);
#if HANYANG
	} else if ( strmatch(tok,"CompositionUnit:")==0 ) {
	    getsint(sfd,&sc->jamo);
	    getsint(sfd,&sc->varient);
	    sc->compositionunit = true;
#endif
	} else if ( strmatch(tok,"HStem:")==0 ) {
	    sc->hstem = SFDReadHints(sfd);
	    sc->hconflicts = StemListAnyConflicts(sc->hstem);
	} else if ( strmatch(tok,"VStem:")==0 ) {
	    sc->vstem = SFDReadHints(sfd);
	    sc->vconflicts = StemListAnyConflicts(sc->vstem);
	} else if ( strmatch(tok,"DStem:")==0 ) {
	    sc->dstem = SFDReadDHints( sc->parent,sfd,true );
            had_old_dstems = true;
	} else if ( strmatch(tok,"DStem2:")==0 ) {
	    sc->dstem = SFDReadDHints( sc->parent,sfd,false );
	} else if ( strmatch(tok,"CounterMasks:")==0 ) {
	    getsint(sfd,&sc->countermask_cnt);
	    sc->countermasks = calloc(sc->countermask_cnt,sizeof(HintMask));
	    for ( i=0; i<sc->countermask_cnt; ++i ) {
		int ch;
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		SFDGetHintMask(sfd,&sc->countermasks[i]);
	    }
	} else if ( strmatch(tok,"AnchorPoint:")==0 ) {
	    lastap = SFDReadAnchorPoints(sfd,sc,&sc->anchor,lastap);
	} else if ( strmatch(tok,"Fore")==0 ) {
	    while ( isspace(ch = nlgetc(sfd)));
	    ungetc(ch,sfd);
	    if ( ch!='I' && ch!='R' && ch!='S' && ch!='V' && ch!=' ' && ch!='\n' && 
	         !PeekMatch(sfd, "Pickled") && !PeekMatch(sfd, "EndChar") &&
	         !PeekMatch(sfd, "Fore") && !PeekMatch(sfd, "Back") && !PeekMatch(sfd, "Layer") ) {
		/* Old format, without a SplineSet token */
		sc->layers[ly_fore].splines = SFDGetSplineSet(sfd,sc->layers[ly_fore].order2);
	    }
	    current_layer = ly_fore;
	    lastgl = NULL;
	} else if ( strmatch(tok,"MinimumDistance:")==0 ) {
	    SFDGetMinimumDistances(sfd,sc);
	} else if ( strmatch(tok,"Validated:")==0 ) {
	    getsint(sfd,(int16 *) &sc->layers[current_layer].validation_state);
	} else if ( strmatch(tok,"Back")==0 ) {
	    while ( isspace(ch=nlgetc(sfd)));
	    ungetc(ch,sfd);
	    if ( ch!='I' && ch!='R' && ch!='S' && ch!='V' && ch!=' ' && ch!='\n' &&
	         !PeekMatch(sfd, "Pickled") && !PeekMatch(sfd, "EndChar") &&
	         !PeekMatch(sfd, "Fore") && !PeekMatch(sfd, "Back") && !PeekMatch(sfd, "Layer") ) {
		/* Old format, without a SplineSet token */
		sc->layers[ly_back].splines = SFDGetSplineSet(sfd,sc->layers[ly_back].order2);
		oldback = true;
	    }
	    current_layer = ly_back;
	    lastgl = NULL;
	} else if ( strmatch(tok,"LayerCount:")==0 ) {
	    getint(sfd,&temp);
	    if ( temp>sc->layer_cnt ) {
		sc->layers = realloc(sc->layers,temp*sizeof(Layer));
		memset(sc->layers+sc->layer_cnt,0,(temp-sc->layer_cnt)*sizeof(Layer));
	    }
	    sc->layer_cnt = temp;
	    current_layer = ly_fore;
	} else if ( strmatch(tok,"Layer:")==0 ) {
	    int layer;
	    int dofill, dostroke, fillfirst, linejoin, linecap;
	    uint32 fillcol, strokecol;
	    real fillopacity, strokeopacity, strokewidth, trans[4];
	    DashType dashes[DASH_MAX];
	    int i;
	    getint(sfd,&layer);
	    if ( layer>=sc->layer_cnt ) {
		sc->layers = realloc(sc->layers,(layer+1)*sizeof(Layer));
		memset(sc->layers+sc->layer_cnt,0,(layer+1-sc->layer_cnt)*sizeof(Layer));
	    }
	    if ( sc->parent->multilayer ) {
		getint(sfd,&dofill);
		getint(sfd,&dostroke);
		getint(sfd,&fillfirst);
		gethex(sfd,&fillcol);
		getreal(sfd,&fillopacity);
		gethex(sfd,&strokecol);
		getreal(sfd,&strokeopacity);
		getreal(sfd,&strokewidth);
		getname(sfd,tok);
		for ( i=0; joins[i]!=NULL; ++i )
		    if ( strmatch(joins[i],tok)==0 )
		break;
		if ( joins[i]==NULL ) --i;
		linejoin = i;
		getname(sfd,tok);
		for ( i=0; caps[i]!=NULL; ++i )
		    if ( strmatch(caps[i],tok)==0 )
		break;
		if ( caps[i]==NULL ) --i;
		linecap = i;
		while ( (ch=nlgetc(sfd))==' ' || ch=='[' );
		ungetc(ch,sfd);
		getreal(sfd,&trans[0]);
		getreal(sfd,&trans[1]);
		getreal(sfd,&trans[2]);
		getreal(sfd,&trans[3]);
		while ( (ch=nlgetc(sfd))==' ' || ch==']' );
		if ( ch=='[' ) {
		    for ( i=0;; ++i ) { int temp;
			if ( !getint(sfd,&temp) )
		    break;
			else if ( i<DASH_MAX )
			    dashes[i] = temp;
		    }
		    if ( i<DASH_MAX )
			dashes[i] = 0;
		} else {
		    ungetc(ch,sfd);
		    memset(dashes,0,sizeof(dashes));
		}
		sc->layers[layer].dofill = dofill;
		sc->layers[layer].dostroke = dostroke;
		sc->layers[layer].fillfirst = fillfirst;
		sc->layers[layer].fill_brush.col = fillcol;
		sc->layers[layer].fill_brush.opacity = fillopacity;
		sc->layers[layer].stroke_pen.brush.col = strokecol;
		sc->layers[layer].stroke_pen.brush.opacity = strokeopacity;
		sc->layers[layer].stroke_pen.width = strokewidth;
		sc->layers[layer].stroke_pen.linejoin = linejoin;
		sc->layers[layer].stroke_pen.linecap = linecap;
		memcpy(sc->layers[layer].stroke_pen.dashes,dashes,sizeof(dashes));
		memcpy(sc->layers[layer].stroke_pen.trans,trans,sizeof(trans));
	    }
	    current_layer = layer;
	    lasti = NULL;
	    lastr = NULL;
	    lastgl = NULL;
	} else if ( strmatch(tok,"FillGradient:")==0 ) {
	    sc->layers[current_layer].fill_brush.gradient = SFDParseGradient(sfd,tok);
	} else if ( strmatch(tok,"FillPattern:")==0 ) {
	    sc->layers[current_layer].fill_brush.pattern = SFDParsePattern(sfd,tok);
	} else if ( strmatch(tok,"StrokeGradient:")==0 ) {
	    sc->layers[current_layer].stroke_pen.brush.gradient = SFDParseGradient(sfd,tok);
	} else if ( strmatch(tok,"StrokePattern:")==0 ) {
	    sc->layers[current_layer].stroke_pen.brush.pattern = SFDParsePattern(sfd,tok);
	} else if ( strmatch(tok,"UndoRedoHistory")==0 ) {

	    getname(sfd,tok);
	    if ( !strmatch(tok,"Layer:") ) {
		int layer;
		getint(sfd,&layer);
	    }

	    int limit;
	    Undoes *undo = 0;
	    struct undoes *last = 0;

	    getname(sfd,tok);
	    if ( !strmatch(tok,"Undoes") ) {
		undo = 0;
		limit = UndoRedoLimitToLoad;
		last = sc->layers[current_layer].undoes;
		while((undo = SFDGetUndo( sfd, sc, "UndoOperation", current_layer )))
		{
		    // push to back
		    if( last ) last->next = undo;
		    else       sc->layers[current_layer].undoes = undo;
		    last = undo;

		    if( limit != -1 ) {
			limit--;
			if( limit <= 0 ) {
			    // we have hit our load limit, so lets just chuck everything away
			    // until we hit the EndUndoes/EndRedoes magic line and then start
			    // actually processing again.
			    const char* terminators[] = { "EndUndoes", "EndRedoes", 0 };
			    SFDConsumeUntil( sfd, terminators );
			}
		    }
		}
	    }
	    getname(sfd,tok);
	    if ( !strmatch(tok,"Redoes") ) {
		undo = 0;
		limit = UndoRedoLimitToLoad;
		last = sc->layers[current_layer].redoes;
		while((undo = SFDGetUndo( sfd, sc, "RedoOperation", current_layer )))
		{
		    // push to back
		    if( last ) last->next = undo;
		    else       sc->layers[current_layer].redoes = undo;
		    last = undo;

		    if( limit != -1 ) {
			limit--;
			if( limit <= 0 ) {
			    // we have hit our load limit, so lets just chuck everything away
			    // until we hit the EndUndoes/EndRedoes magic line and then start
			    // actually processing again.
			    const char* terminators[] = { "EndUndoes", "EndRedoes", 0 };
			    SFDConsumeUntil( sfd, terminators );
			}
		    }
		}
	    }
	} else if ( strmatch(tok,"SplineSet")==0 ) {
	    sc->layers[current_layer].splines = SFDGetSplineSet(sfd,sc->layers[current_layer].order2);
	} else if ( strmatch(tok,"Guideline:")==0 ) {
	    lastgl = SFDReadGuideline(sfd, &sc->layers[current_layer].guidelines, lastgl);
	} else if ( strmatch(tok,"Ref:")==0 || strmatch(tok,"Refer:")==0 ) {
	    /* I should be depending on the version number here, but I made */
	    /*  a mistake and bumped the version too late. So the version is */
	    /*  not an accurate mark, but the presence of a LayerCount keyword*/
	    /*  in the font is an good mark. Before the LayerCount was added */
	    /*  (version 2) only the foreground layer could have references */
	    /*  after that (eventually version 3) any layer could. */
	    if ( oldback || !had_sf_layer_cnt ) current_layer = ly_fore;
	    ref = SFDGetRef(sfd,strmatch(tok,"Ref:")==0);
	    if ( sc->layers[current_layer].refs==NULL )
		sc->layers[current_layer].refs = ref;
	    else
		lastr->next = ref;
	    lastr = ref;
	} else if ( strmatch(tok,"Image:")==0 ) {
	    int ly = current_layer;
	    if ( !multilayer && !sc->layers[ly].background ) ly = ly_back;
	    img = SFDGetImage(sfd);
	    if (img != NULL) {
	    if ( sc->layers[ly].images==NULL )
		sc->layers[ly].images = img;
	    else
		lasti->next = img;
	    lasti = img;
	    }
	} else if ( strmatch(tok,"Image2:")==0 ) {
#ifndef _NO_LIBPNG
	    enum MIME mime = SFDGetImage2MIME(sfd);
	    if (mime == PNG) {
		int ly = current_layer;
		if ( !multilayer && !sc->layers[ly].background ) ly = ly_back;
		img = SFDGetImagePNG(sfd);
		if (img != NULL) {
		    if ( sc->layers[ly].images==NULL )
			sc->layers[ly].images = img;
		    else
			lasti->next = img;
		    lasti = img;
		}
	    } else
#endif
	    {
	    LogError(_("Image2 skipped as it uses an unsupported image type"));
	    const char* im2_terminator[] = { "EndImage2", 0 };
	    SFDConsumeUntil(sfd, im2_terminator);
	    }
	} else if ( strmatch(tok,"PickledData:")==0 ) {
	    if (current_layer < sc->layer_cnt) {
	      sc->layers[current_layer].python_persistent = SFDUnPickle(sfd, 0);
	      sc->layers[current_layer].python_persistent_has_lists = 0;
	    }
	} else if ( strmatch(tok,"PickledDataWithLists:")==0 ) {
	    if (current_layer < sc->layer_cnt) {
	      sc->layers[current_layer].python_persistent = SFDUnPickle(sfd, 1);
	      sc->layers[current_layer].python_persistent_has_lists = 1;
	    }
	} else if ( strmatch(tok,"OrigType1:")==0 ) {	/* Accept, slurp, ignore contents */
	    SFDGetType1(sfd);
	} else if ( strmatch(tok,"TtfInstrs:")==0 ) {	/* Binary format */
	    SFDGetTtfInstrs(sfd,sc);
	} else if ( strmatch(tok,"TtInstrs:")==0 ) {	/* ASCII format */
	    SFDGetTtInstrs(sfd,sc);
	} else if ( strmatch(tok,"Kerns2:")==0 ||
		strmatch(tok,"VKerns2:")==0 ) {
	    KernPair *kp, *last=NULL;
	    int isv = *tok=='V';
	    int off, index;
	    struct lookup_subtable *sub;

	    if ( sf->sfd_version<2 )
		LogError(_("Found an new style kerning pair inside a version 1 (or lower) sfd file.\n") );
	    while ( fscanf(sfd,"%d %d", &index, &off )==2 ) {
		sub = SFFindLookupSubtableAndFreeName(sf,SFDReadUTF7Str(sfd));
		if ( sub==NULL ) {
		    LogError(_("KernPair with no subtable name.\n"));
	    break;
		}
		kp = chunkalloc(sizeof(KernPair1));
		kp->sc = (SplineChar *) (intpt) index;
		kp->kcid = true;
		kp->off = off;
		kp->subtable = sub;
		kp->next = NULL;
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		if ( ch=='{' ) {
		    kp->adjust = SFDReadDeviceTable(sfd, NULL);
		}
		if ( last != NULL )
		    last->next = kp;
		else if ( isv )
		    sc->vkerns = kp;
		else
		    sc->kerns = kp;
		last = kp;
	    }
	} else if ( strmatch(tok,"Kerns:")==0 ||
		strmatch(tok,"KernsSLI:")==0 ||
		strmatch(tok,"KernsSLIF:")==0 ||
		strmatch(tok,"VKernsSLIF:")==0 ||
		strmatch(tok,"KernsSLIFO:")==0 ||
		strmatch(tok,"VKernsSLIFO:")==0 ) {
	    KernPair1 *kp, *last=NULL;
	    int index, off, sli, flags=0;
	    int hassli = (strmatch(tok,"KernsSLI:")==0);
	    int isv = *tok=='V';
	    int has_orig = strstr(tok,"SLIFO:")!=NULL;
	    if ( sf->sfd_version>=2 ) {
		IError( "Found an old style kerning pair inside a version 2 (or higher) sfd file." );
exit(1);
	    }
	    if ( strmatch(tok,"KernsSLIF:")==0 || strmatch(tok,"KernsSLIFO:")==0 ||
		    strmatch(tok,"VKernsSLIF:")==0 || strmatch(tok,"VKernsSLIFO:")==0 )
		hassli=2;
	    while ( (hassli==1 && fscanf(sfd,"%d %d %d", &index, &off, &sli )==3) ||
		    (hassli==2 && fscanf(sfd,"%d %d %d %d", &index, &off, &sli, &flags )==4) ||
		    (hassli==0 && fscanf(sfd,"%d %d", &index, &off )==2) ) {
		if ( !hassli )
		    sli = SFFindBiggestScriptLangIndex(sli_sf,
			    script!=0?script:SCScriptFromUnicode(sc),DEFAULT_LANG);
		if ( sli>=((SplineFont1 *) sli_sf)->sli_cnt && sli!=SLI_NESTED) {
		    static int complained=false;
		    if ( !complained )
			IError("'%s' in %s has a script index out of bounds: %d",
				isv ? "vkrn" : "kern",
				sc->name, sli );
		    sli = SFFindBiggestScriptLangIndex(sli_sf,
			    SCScriptFromUnicode(sc),DEFAULT_LANG);
		    complained = true;
		}
		kp = chunkalloc(sizeof(KernPair1));
		kp->kp.sc = (SplineChar *) (intpt) index;
		kp->kp.kcid = has_orig;
		kp->kp.off = off;
		kp->sli = sli;
		kp->flags = flags;
		kp->kp.next = NULL;
		while ( (ch=nlgetc(sfd))==' ' );
		ungetc(ch,sfd);
		if ( ch=='{' ) {
		    kp->kp.adjust = SFDReadDeviceTable(sfd, NULL);
		}
		if ( last != NULL )
		    last->kp.next = (KernPair *) kp;
		else if ( isv )
		    sc->vkerns = (KernPair *) kp;
		else
		    sc->kerns = (KernPair *) kp;
		last = kp;
	    }
	} else if ( (ispos = (strmatch(tok,"Position:")==0)) ||
		( ispos  = (strmatch(tok,"Position2:")==0)) ||
		( ispair = (strmatch(tok,"PairPos:")==0)) ||
		( ispair = (strmatch(tok,"PairPos2:")==0)) ||
		( islcar = (strmatch(tok,"LCarets:")==0)) ||
		( islcar = (strmatch(tok,"LCarets2:")==0)) ||
		( isliga = (strmatch(tok,"Ligature:")==0)) ||
		( isliga = (strmatch(tok,"Ligature2:")==0)) ||
		( issubs = (strmatch(tok,"Substitution:")==0)) ||
		( issubs = (strmatch(tok,"Substitution2:")==0)) ||
		( ismult = (strmatch(tok,"MultipleSubs:")==0)) ||
		( ismult = (strmatch(tok,"MultipleSubs2:")==0)) ||
		strmatch(tok,"AlternateSubs:")==0 ||
		strmatch(tok,"AlternateSubs2:")==0 ) {
	    PST *pst;
	    int old, type;
	    type = ispos ? pst_position :
			 ispair ? pst_pair :
			 islcar ? pst_lcaret :
			 isliga ? pst_ligature :
			 issubs ? pst_substitution :
			 ismult ? pst_multiple :
			 pst_alternate;
	    if ( strchr(tok,'2')!=NULL ) {
		old = false;
		pst = chunkalloc(sizeof(PST));
		if ( type!=pst_lcaret )
		    pst->subtable = SFFindLookupSubtableAndFreeName(sf,SFDReadUTF7Str(sfd));
	    } else {
		old = true;
		pst = chunkalloc(sizeof(PST1));
		((PST1 *) pst)->tag = CHR('l','i','g','a');
		((PST1 *) pst)->script_lang_index = 0xffff;
		while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
		if ( isdigit(ch)) {
		    int temp;
		    ungetc(ch,sfd);
		    getint(sfd,&temp);
		    ((PST1 *) pst)->flags = temp;
		    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
		} else
		    ((PST1 *) pst)->flags = 0 /*PSTDefaultFlags(type,sc)*/;
		if ( isdigit(ch)) {
		    ungetc(ch,sfd);
		    getusint(sfd,&((PST1 *) pst)->script_lang_index);
		    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
		} else
		    ((PST1 *) pst)->script_lang_index = SFFindBiggestScriptLangIndex(sf,
			    script!=0?script:SCScriptFromUnicode(sc),DEFAULT_LANG);
		if ( ch=='\'' ) {
		    ungetc(ch,sfd);
		    ((PST1 *) pst)->tag = gettag(sfd);
		} else if ( ch=='<' ) {
		    getint(sfd,&temp);
		    ((PST1 *) pst)->tag = temp<<16;
		    nlgetc(sfd);	/* comma */
		    getint(sfd,&temp);
		    ((PST1 *) pst)->tag |= temp;
		    nlgetc(sfd);	/* close '>' */
		    ((PST1 *) pst)->macfeature = true;
		} else
		    ungetc(ch,sfd);
		if ( type==pst_lcaret ) {
		/* These are meaningless for lcarets, set them to innocuous values */
		    ((PST1 *) pst)->script_lang_index = SLI_UNKNOWN;
		    ((PST1 *) pst)->tag = CHR(' ',' ',' ',' ');
		} else if ( ((PST1 *) pst)->script_lang_index>=((SplineFont1 *) sli_sf)->sli_cnt && ((PST1 *) pst)->script_lang_index!=SLI_NESTED ) {
		    static int complained=false;
		    if ( !complained )
			IError("'%c%c%c%c' in %s has a script index out of bounds: %d",
				(((PST1 *) pst)->tag>>24), (((PST1 *) pst)->tag>>16)&0xff, (((PST1 *) pst)->tag>>8)&0xff, ((PST1 *) pst)->tag&0xff,
				sc->name, ((PST1 *) pst)->script_lang_index );
		    else
			IError( "'%c%c%c%c' in %s has a script index out of bounds: %d\n",
				(((PST1 *) pst)->tag>>24), (((PST1 *) pst)->tag>>16)&0xff, (((PST1 *) pst)->tag>>8)&0xff, ((PST1 *) pst)->tag&0xff,
				sc->name, ((PST1 *) pst)->script_lang_index );
		    ((PST1 *) pst)->script_lang_index = SFFindBiggestScriptLangIndex(sli_sf,
			    SCScriptFromUnicode(sc),DEFAULT_LANG);
		    complained = true;
		}
	    }
	    if ( (sf->sfd_version<2)!=old ) {
		IError( "Version mixup in PST of sfd file." );
exit(1);
	    }
	    if ( last==NULL )
		sc->possub = pst;
	    else
		last->next = pst;
	    last = pst;
	    pst->type = type;
	    if ( pst->type==pst_position ) {
		fscanf( sfd, " dx=%hd dy=%hd dh=%hd dv=%hd",
			&pst->u.pos.xoff, &pst->u.pos.yoff,
			&pst->u.pos.h_adv_off, &pst->u.pos.v_adv_off);
		pst->u.pos.adjust = SFDReadValDevTab(sfd);
		ch = nlgetc(sfd);		/* Eat new line */
	    } else if ( pst->type==pst_pair ) {
		getname(sfd,tok);
		pst->u.pair.paired = copy(tok);
		pst->u.pair.vr = chunkalloc(sizeof(struct vr [2]));
		fscanf( sfd, " dx=%hd dy=%hd dh=%hd dv=%hd",
			&pst->u.pair.vr[0].xoff, &pst->u.pair.vr[0].yoff,
			&pst->u.pair.vr[0].h_adv_off, &pst->u.pair.vr[0].v_adv_off);
		pst->u.pair.vr[0].adjust = SFDReadValDevTab(sfd);
		fscanf( sfd, " dx=%hd dy=%hd dh=%hd dv=%hd",
			&pst->u.pair.vr[1].xoff, &pst->u.pair.vr[1].yoff,
			&pst->u.pair.vr[1].h_adv_off, &pst->u.pair.vr[1].v_adv_off);
		pst->u.pair.vr[0].adjust = SFDReadValDevTab(sfd);
		ch = nlgetc(sfd);
	    } else if ( pst->type==pst_lcaret ) {
		int i;
		fscanf( sfd, " %d", &pst->u.lcaret.cnt );
		pst->u.lcaret.carets = malloc(pst->u.lcaret.cnt*sizeof(int16));
		for ( i=0; i<pst->u.lcaret.cnt; ++i )
		    fscanf( sfd, " %hd", &pst->u.lcaret.carets[i]);
		geteol(sfd,tok);
	    } else {
		geteol(sfd,tok);
		pst->u.lig.components = copy(tok);	/* it's in the same place for all formats */
		if ( isliga ) {
		    pst->u.lig.lig = sc;
		    if ( old )
			last = (PST *) LigaCreateFromOldStyleMultiple((PST1 *) pst);
		}
	    }
#ifdef FONTFORGE_CONFIG_CVT_OLD_MAC_FEATURES
	    if ( old )
		CvtOldMacFeature((PST1 *) pst);
#endif
	} else if ( strmatch(tok,"Colour:")==0 ) {
	    uint32 temp;
	    gethex(sfd,&temp);
	    sc->color = temp;
	} else if ( strmatch(tok,"Comment:")==0 ) {
	    sc->comment = SFDReadUTF7Str(sfd);
	} else if ( strmatch(tok,"Decomposition:")==0 ) {
	    char* decomp = SFDReadUTF7Str(sfd);
	    sc->user_decomp = utf82u_copy(decomp);
	    free(decomp);
	} else if ( strmatch(tok,"TileMargin:")==0 ) {
	    getreal(sfd,&sc->tile_margin);
	} else if ( strmatch(tok,"TileBounds:")==0 ) {
	    getreal(sfd,&sc->tile_bounds.minx);
	    getreal(sfd,&sc->tile_bounds.miny);
	    getreal(sfd,&sc->tile_bounds.maxx);
	    getreal(sfd,&sc->tile_bounds.maxy);
	} else if ( strmatch(tok,"EndChar")==0 ) {
	    if ( sc->orig_pos<sf->glyphcnt )
		sf->glyphs[sc->orig_pos] = sc;
            /* Recalculating hint active zones may be needed for old .sfd files. */
            /* Do this when we have finished with other glyph components, */
            /* so that splines are already available */
	    if ( sf->sfd_version<2 )
                SCGuessHintInstancesList( sc,ly_fore,sc->hstem,sc->vstem,sc->dstem,false,false );
            else if ( had_old_dstems && sc->layers[ly_fore].splines != NULL )
                SCGuessHintInstancesList( sc,ly_fore,NULL,NULL,sc->dstem,false,true );
	    if ( sc->layers[ly_fore].order2 )
		SCDefaultInterpolation(sc);
return( sc );
	} else {
	    geteol(sfd,tok);
	}
    }
}