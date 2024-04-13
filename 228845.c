void SFDGetPSTs( FILE *sfd, SplineChar *sc, char* ttok ) {
    struct splinefont * sf = sc->parent;
    char tok[2001], ch;
    int isliga = 0, ispos, issubs, ismult, islcar, ispair, temp;
    PST *last = NULL;
    uint32 script = 0;
    SplineFont *sli_sf = sf->cidmaster ? sf->cidmaster : sf;

    strncpy(tok,ttok,sizeof(tok)-1);

    if ( strmatch(tok,"Script:")==0 ) {
	/* Obsolete. But still used for parsing obsolete ligature/subs tags */
	while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
	if ( ch=='\n' || ch=='\r' )
	    script = 0;
	else {
	    ungetc(ch,sfd);
	    script = gettag(sfd);
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
    } else {
	return;
    }

    // we matched something, grab the next top level token to ttok
    getname( sfd, ttok );
}