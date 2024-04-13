static void SFDParseLookup(FILE *sfd,OTLookup *otl) {
    int ch;
    struct lookup_subtable *sub, *lastsub;
    FeatureScriptLangList *fl, *lastfl;
    struct scriptlanglist *sl, *lastsl;
    int i, lcnt, lmax=0;
    uint32 *langs=NULL;
    char *subname;

    while ( (ch=nlgetc(sfd))==' ' );
    if ( ch=='{' ) {
	lastsub = NULL;
	while ( (subname = SFDReadUTF7Str(sfd))!=NULL ) {
	    while ( (ch=nlgetc(sfd))==' ' );
	    ungetc(ch,sfd);
	    sub = chunkalloc(sizeof(struct lookup_subtable));
	    sub->subtable_name = subname;
	    sub->lookup = otl;
	    switch ( otl->lookup_type ) {
	      case gsub_single:
		while ( (ch=nlgetc(sfd))==' ' );
		if ( ch=='(' ) {
		    sub->suffix = SFDReadUTF7Str(sfd);
		    while ( (ch=nlgetc(sfd))==' ' );
			/* slurp final paren */
		} else
		    ungetc(ch,sfd);
		sub->per_glyph_pst_or_kern = true;
	      break;
	      case gsub_multiple: case gsub_alternate: case gsub_ligature:
	      case gpos_single:
		sub->per_glyph_pst_or_kern = true;
	      break;
	      case gpos_pair:
		if ( (ch=nlgetc(sfd))=='(' ) {
		    ch = nlgetc(sfd);
		    sub->vertical_kerning = (ch=='1');
		    nlgetc(sfd);	/* slurp final paren */
		    ch=nlgetc(sfd);
		}
		if ( ch=='[' ) {
		    getsint(sfd,&sub->separation);
		    nlgetc(sfd);	/* slurp comma */
		    getsint(sfd,&sub->minkern);
		    nlgetc(sfd);	/* slurp comma */
		    ch = nlgetc(sfd);
		    sub->kerning_by_touch = ((ch-'0')&1)?1:0;
		    sub->onlyCloser       = ((ch-'0')&2)?1:0;
		    sub->dontautokern     = ((ch-'0')&4)?1:0;
		    nlgetc(sfd);	/* slurp final bracket */
		} else {
		    ungetc(ch,sfd);
		}
		sub->per_glyph_pst_or_kern = true;
	      break;
	      case gpos_cursive: case gpos_mark2base: case gpos_mark2ligature: case gpos_mark2mark:
		sub->anchor_classes = true;
	      break;
	      default:
	      break;
	    }
	    if ( lastsub==NULL )
		otl->subtables = sub;
	    else
		lastsub->next = sub;
	    lastsub = sub;
	}
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch=='}' )
	    ch = nlgetc(sfd);
    }
    while ( ch==' ' )
	ch = nlgetc(sfd);
    if ( ch=='[' ) {
	lastfl = NULL;
	for (;;) {
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch==']' )
	break;
	    fl = chunkalloc(sizeof(FeatureScriptLangList));
	    if ( lastfl==NULL )
		otl->features = fl;
	    else
		lastfl->next = fl;
	    lastfl = fl;
	    if ( ch=='<' ) {
		int ft=0,fs=0;
		fscanf(sfd,"%d,%d>", &ft, &fs );
		fl->ismac = true;
		fl->featuretag = (ft<<16) | fs;
	    } else if ( ch=='\'' ) {
		ungetc(ch,sfd);
		fl->featuretag = gettag(sfd);
	    }
	    while ( (ch=nlgetc(sfd))==' ' );
	    if ( ch=='(' ) {
		lastsl = NULL;
		for (;;) {
		    while ( (ch=nlgetc(sfd))==' ' );
		    if ( ch==')' )
		break;
		    sl = chunkalloc(sizeof(struct scriptlanglist));
		    if ( lastsl==NULL )
			fl->scripts = sl;
		    else
			lastsl->next = sl;
		    lastsl = sl;
		    if ( ch=='\'' ) {
			ungetc(ch,sfd);
			sl->script = gettag(sfd);
		    }
		    while ( (ch=nlgetc(sfd))==' ' );
		    if ( ch=='<' ) {
			lcnt = 0;
			for (;;) {
			    while ( (ch=nlgetc(sfd))==' ' );
			    if ( ch=='>' )
			break;
			    if ( ch=='\'' ) {
				ungetc(ch,sfd);
			        if ( lcnt>=lmax )
				    langs = realloc(langs,(lmax+=10)*sizeof(uint32));
				langs[lcnt++] = gettag(sfd);
			    }
			}
			sl->lang_cnt = lcnt;
			if ( lcnt>MAX_LANG )
			    sl->morelangs = malloc((lcnt-MAX_LANG)*sizeof(uint32));
			for ( i=0; i<lcnt; ++i ) {
			    if ( i<MAX_LANG )
				sl->langs[i] = langs[i];
			    else
				sl->morelangs[i-MAX_LANG] = langs[i];
			}
		    }
		}
	    }
	}
    }
    free(langs);
}