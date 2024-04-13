void SFDFixupRefs(SplineFont *sf) {
    int i, isv;
    RefChar *refs, *rnext, *rprev;
    /*int isautorecovery = sf->changed;*/
    KernPair *kp, *prev, *next;
    EncMap *map = sf->map;
    int layer;
    int k,l;
    SplineFont *cidmaster = sf, *ksf;

    k = 1;
    if ( sf->subfontcnt!=0 )
	sf = sf->subfonts[0];

    ff_progress_change_line2(_("Interpreting Glyphs"));
    for (;;) {
	for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	    SplineChar *sc = sf->glyphs[i];
	    /* A changed character is one that has just been recovered */
	    /*  unchanged characters will already have been fixed up */
	    /* Er... maybe not. If the character being recovered is refered to */
	    /*  by another character then we need to fix up that other char too*/
	    /*if ( isautorecovery && !sc->changed )*/
	/*continue;*/
	    for ( layer = 0; layer<sc->layer_cnt; ++layer ) {
		rprev = NULL;
		for ( refs = sc->layers[layer].refs; refs!=NULL; refs=rnext ) {
		    rnext = refs->next;
		    if ( refs->encoded ) {		/* Old sfd format */
			if ( refs->orig_pos<map->encmax && map->map[refs->orig_pos]!=-1 )
			    refs->orig_pos = map->map[refs->orig_pos];
			else
			    refs->orig_pos = sf->glyphcnt;
			refs->encoded = false;
		    }
		    if ( refs->orig_pos<sf->glyphcnt && refs->orig_pos>=0 )
			refs->sc = sf->glyphs[refs->orig_pos];
		    if ( refs->sc!=NULL ) {
			refs->unicode_enc = refs->sc->unicodeenc;
			refs->adobe_enc = getAdobeEnc(refs->sc->name);
			rprev = refs;
			if ( refs->use_my_metrics ) {
			    if ( sc->width != refs->sc->width ) {
				LogError(_("Bad sfd file. Glyph %s has width %d even though it should be\n  bound to the width of %s which is %d.\n"),
					sc->name, sc->width, refs->sc->name, refs->sc->width );
				sc->width = refs->sc->width;
			    }
			}
		    } else {
			RefCharFree(refs);
			if ( rprev!=NULL )
			    rprev->next = rnext;
			else
			    sc->layers[layer].refs = rnext;
		    }
		}
	    }
	    /* In old sfd files we used a peculiar idiom to represent a multiply */
	    /*  encoded glyph. Fix it up now. Remove the fake glyph and adjust the*/
	    /*  map */
	    /*if ( isautorecovery && !sc->changed )*/
	/*continue;*/
	    for ( isv=0; isv<2; ++isv ) {
		for ( prev = NULL, kp=isv?sc->vkerns : sc->kerns; kp!=NULL; kp=next ) {
		    int index = (intpt) (kp->sc);

		    next = kp->next;
		    // be impotent if the reference is already to the correct location
                    if ( !kp->kcid ) {	/* It's encoded (old sfds), else orig */
                        if ( index>=map->encmax || map->map[index]==-1 )
                            index = sf->glyphcnt;
                        else
                            index = map->map[index];
                    }
                    kp->kcid = false;
                    ksf = sf;
                    if ( cidmaster!=sf ) {
                        for ( l=0; l<cidmaster->subfontcnt; ++l ) {
                            ksf = cidmaster->subfonts[l];
                            if ( index<ksf->glyphcnt && ksf->glyphs[index]!=NULL )
                                break;
                        }
                    }
                    if ( index>=ksf->glyphcnt || ksf->glyphs[index]==NULL ) {
                        IError( "Bad kerning information in glyph %s\n", sc->name );
                        kp->sc = NULL;
                    } else {
                        kp->sc = ksf->glyphs[index];
                    }

		    if ( kp->sc!=NULL )
			prev = kp;
		    else{
			if ( prev!=NULL )
			    prev->next = next;
			else if ( isv )
			    sc->vkerns = next;
			else
			    sc->kerns = next;
			chunkfree(kp,sizeof(KernPair));
		    }
		}
	    }
	    if ( SCDuplicate(sc)!=sc ) {
		SplineChar *base = SCDuplicate(sc);
		int orig = sc->orig_pos, enc = sf->map->backmap[orig], uni = sc->unicodeenc;
		SplineCharFree(sc);
		sf->glyphs[i]=NULL;
		sf->map->backmap[orig] = -1;
		sf->map->map[enc] = base->orig_pos;
		AltUniAdd(base,uni);
	    }
	}
	for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	    SplineChar *sc = sf->glyphs[i];
	    for ( layer=0; layer<sc->layer_cnt; ++layer ) {
		for ( refs = sf->glyphs[i]->layers[layer].refs; refs!=NULL; refs=refs->next ) {
		    SFDFixupRef(sf->glyphs[i],refs,layer);
		}
	    }
	    ff_progress_next();
	}
	if ( sf->cidmaster==NULL )
	    for ( i=sf->glyphcnt-1; i>=0 && sf->glyphs[i]==NULL; --i )
		sf->glyphcnt = i;
	if ( k>=cidmaster->subfontcnt )
    break;
	sf = cidmaster->subfonts[k++];
    }
}