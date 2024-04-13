Undoes *SFDGetUndo( FILE *sfd, SplineChar *sc,
		    const char* startTag,
		    int current_layer )
{
    Undoes *u = 0;
    char tok[2000];
    int i;
    RefChar *lastr=NULL;
    ImageList *lasti=NULL;
    AnchorPoint *lastap = NULL;
    GuidelineSet *lastgl = NULL;
    SplineChar* tsc = 0;

    if ( getname(sfd,tok)!=1 )
        return( NULL );
    if ( strcmp(tok, startTag) )
        return( NULL );

    u = chunkalloc(sizeof(Undoes));
    u->undotype = ut_state;
    u->layer = UNDO_LAYER_UNKNOWN;

    while ( 1 )
    {
        if ( getname(sfd,tok)!=1 ) {
            chunkfree(u,sizeof(Undoes));
            return( NULL );
        }

        if ( !strmatch(tok,"EndUndoOperation")
            || !strmatch(tok,"EndRedoOperation"))
        {
            if( u->undotype == ut_hints ) {
                if( tsc ) {
                    u->u.state.hints = UHintCopy(tsc,1);
                    SplineCharFree( tsc );
                }
            }

            return u;
        }
	if ( !strmatch(tok,"Index:")) {
            getint(sfd,&i);
        }
	if ( !strmatch(tok,"Type:")) {
            getint(sfd,&i);
            u->undotype = i;
            if( u->undotype == ut_hints ) {
                tsc = SplineCharCopy( sc, 0, 0 );
                tsc->hstem = 0;
                tsc->vstem = 0;
                tsc->dstem = 0;
            }
        }
        if ( !strmatch(tok,"WasModified:")) {
            getint(sfd,&i);
            u->was_modified = i;
        }
        if ( !strmatch(tok,"WasOrder2:")) {
            getint(sfd,&i);
            u->was_order2 = i;
        }
        if ( !strmatch(tok,"Layer:")) {
            getint(sfd,&i);
            u->layer = i;
        }

        switch( u->undotype )
        {
	case ut_tstate:
	case ut_state:
	    if ( !strmatch(tok,"Width:"))          { getint(sfd,&i); u->u.state.width = i; }
	    if ( !strmatch(tok,"VWidth:"))         { getint(sfd,&i); u->u.state.vwidth = i; }
	    if ( !strmatch(tok,"LBearingChange:")) { getint(sfd,&i); u->u.state.lbearingchange = i; }
	    if ( !strmatch(tok,"UnicodeEnc:"))     { getint(sfd,&i); u->u.state.unicodeenc = i; }
	    if ( !strmatch(tok,"Charname:"))       { u->u.state.charname = getquotedeol(sfd); }
	    if ( !strmatch(tok,"Comment:"))        { u->u.state.comment  = getquotedeol(sfd); }

	    if( !strmatch(tok,"Refer:"))
	    {
		RefChar *ref = SFDGetRef(sfd,strmatch(tok,"Ref:")==0);
		int i=0;
		for( i=0; i< ref->layer_cnt; i++ ) {
		    ref->layers[i].splines = 0;
		}
		if ( !u->u.state.refs )
		    u->u.state.refs = ref;
		else
		    lastr->next = ref;
		lastr = ref;
	    }

	    if( !strmatch(tok,"Image:"))
	    {
		ImageList *img = SFDGetImage(sfd);
		if (img != NULL) {
		if ( !u->u.state.images )
		    u->u.state.images = img;
		else
		    lasti->next = img;
		lasti = img;
		}
	    }

	    if( !strmatch(tok,"Image2:"))
	    {
#ifndef _NO_LIBPNG
		enum MIME mime = SFDGetImage2MIME(sfd);
		if (mime == PNG) {
		    ImageList *img = SFDGetImagePNG(sfd);
		    if (img != NULL) {
			if ( !u->u.state.images )
			    u->u.state.images = img;
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
	    }

	    if( !strmatch(tok,"Comment:")) {
		u->u.state.comment  = getquotedeol(sfd);
	    }
	    if( !strmatch(tok,"InstructionsLength:")) {
		getint(sfd,&i); u->u.state.instrs_len = i;
	    }
	    if( !strmatch(tok,"AnchorPoint:") ) {
		lastap = SFDReadAnchorPoints( sfd, sc, &(u->u.state.anchor), lastap );
	    }
	    if ( !strmatch(tok,"SplineSet")) {
		u->u.state.splines = SFDGetSplineSet(sfd,sc->layers[current_layer].order2);
	    }
	    break;
	case ut_hints:
	{
	    if ( !strmatch(tok,"HStem:") ) {
		tsc->hstem = SFDReadHints(sfd);
		tsc->hconflicts = StemListAnyConflicts(tsc->hstem);
	    }
	    else if ( !strmatch(tok,"VStem:") ) {
		tsc->vstem = SFDReadHints(sfd);
		tsc->vconflicts = StemListAnyConflicts(tsc->vstem);
	    }
	    else if( !strmatch(tok,"DStem2:"))
		tsc->dstem = SFDReadDHints( sc->parent,sfd,false );
	    else if( !strmatch(tok,"TtInstrs:")) {
		SFDGetTtInstrs(sfd,tsc);
		u->u.state.instrs = tsc->ttf_instrs;
		u->u.state.instrs_len = tsc->ttf_instrs_len;
		tsc->ttf_instrs = 0;
		tsc->ttf_instrs_len = 0;
	    }
	    break;
	}

	case ut_width:
	case ut_vwidth:
	    if( !strmatch(tok,"Width:")) {
		getint(sfd,&i); u->u.width = i;
	    }
	    break;
	default:
	break;
        }
    }

    return u;
}