static SplineSet *SFDGetSplineSet(FILE *sfd,int order2) {
    SplinePointList *cur=NULL, *head=NULL;
    BasePoint current;
    real stack[100];
    int sp=0;
    SplinePoint *pt = NULL;
    int ch;
	int ch2;
    char tok[100];
    int ttfindex = 0;
    int lastacceptable;
    int flags = 0, tmp;

    current.x = current.y = 0;
    lastacceptable = 0;
    while ( 1 ) {

	while ( getreal(sfd,&stack[sp])==1 )
	    if ( sp<99 )
		++sp;
	while ( isspace(ch=nlgetc(sfd)));
	if ( ch=='E' || ch=='e' || ch==EOF )
    break;
	if ( ch=='S' ) {
	    ungetc(ch,sfd);
	    SFDGetSpiros(sfd,cur);
    continue;
	} else if (( ch=='N' ) &&
	    nlgetc(sfd)=='a' &&	/* a */
	    nlgetc(sfd)=='m' &&	/* m */
	    nlgetc(sfd)=='e' &&	/* e */
	    nlgetc(sfd)=='d' ) /* d */ {
	    ch2 = nlgetc(sfd);		/* : */
		// We are either fetching a splineset name (Named:) or a point name (NamedP:).
		if (ch2=='P') { if ((nlgetc(sfd)==':') && (pt!=NULL)) { if (pt->name!=NULL) {free(pt->name);} pt->name = SFDReadUTF7Str(sfd); } }
		else if (ch2==':') { if (cur != NULL) cur->contour_name = SFDReadUTF7Str(sfd); else { char * freetmp = SFDReadUTF7Str(sfd); free(freetmp); freetmp = NULL; } }
        continue;
	} else if ( ch=='P' && PeekMatch(sfd,"ath") ) {
	    int flags;
	    nlgetc(sfd);		/* a */
	    nlgetc(sfd);		/* t */
	    nlgetc(sfd);		/* h */
	    if (PeekMatch(sfd,"Flags:")) {
	      nlgetc(sfd);		/* F */
	      nlgetc(sfd);		/* l */
	      nlgetc(sfd);		/* a */
	      nlgetc(sfd);		/* g */
	      nlgetc(sfd);		/* s */
	      nlgetc(sfd);		/* : */
	      getint(sfd,&flags);
	      if (cur != NULL) cur->is_clip_path = flags&1;
	    } else if (PeekMatch(sfd,"Start:")) {
	      nlgetc(sfd);		/* S */
	      nlgetc(sfd);		/* t */
	      nlgetc(sfd);		/* a */
	      nlgetc(sfd);		/* r */
	      nlgetc(sfd);		/* t */
	      nlgetc(sfd);		/* : */
	      getint(sfd,&flags);
	      if (cur != NULL) cur->start_offset = flags;
	    }
	}
	pt = NULL;
	if ( ch=='l' || ch=='m' ) {
	    if ( sp>=2 ) {
		current.x = stack[sp-2];
		current.y = stack[sp-1];
		sp -= 2;
		pt = chunkalloc(sizeof(SplinePoint));
		pt->me = current;
		pt->noprevcp = true; pt->nonextcp = true;
		if ( ch=='m' ) {
		    SplinePointList *spl = chunkalloc(sizeof(SplinePointList));
		    spl->first = spl->last = pt;
		    spl->start_offset = 0;
		    if ( cur!=NULL ) {
			if ( !(flags & SFD_PTFLAG_FORCE_OPEN_PATH) && SFDCloseCheck(cur,order2) )
			    --ttfindex;
			cur->next = spl;
		    } else
			head = spl;
		    cur = spl;
		} else {
		    if ( cur!=NULL && cur->first!=NULL && (cur->first!=cur->last || cur->first->next==NULL) ) {
			if ( cur->last->nextcpindex==0xfffe )
			    cur->last->nextcpindex = 0xffff;
			SplineMake(cur->last,pt,order2);
			cur->last->nonextcp = 1;
			pt->noprevcp = 1;
			cur->last = pt;
		    }
		}
	    } else
		sp = 0;
	} else if ( ch=='c' ) {
	    if ( sp>=6 ) {
		current.x = stack[sp-2];
		current.y = stack[sp-1];

		if ( cur!=NULL && cur->first!=NULL && (cur->first!=cur->last || cur->first->next==NULL) ) {
		    cur->last->nextcp.x = stack[sp-6];
		    cur->last->nextcp.y = stack[sp-5];
		    cur->last->nonextcp = false;
		    pt = chunkalloc(sizeof(SplinePoint));
		    pt->prevcp.x = stack[sp-4];
		    pt->prevcp.y = stack[sp-3];
		    pt->me = current;
		    pt->nonextcp = true;
		    if ( cur->last->nextcpindex==0xfffe )
			cur->last->nextcpindex = ttfindex++;
		    else if ( cur->last->nextcpindex!=0xffff )
			ttfindex = cur->last->nextcpindex+1;
		    SplineMake(cur->last,pt,order2);
		    cur->last = pt;
		}

		sp -= 6;
	    } else
		sp = 0;
	}
	if ( pt!=NULL ) {
	    getint(sfd,&flags);

	    pt->pointtype = (flags & SFD_PTFLAG_TYPE_MASK);
	    pt->selected  = (flags & SFD_PTFLAG_IS_SELECTED) > 0;
	    pt->nextcpdef = (flags & SFD_PTFLAG_NEXTCP_IS_DEFAULT) > 0;
	    pt->prevcpdef = (flags & SFD_PTFLAG_PREVCP_IS_DEFAULT) > 0;
	    pt->roundx    = (flags & SFD_PTFLAG_ROUND_IN_X) > 0;
	    pt->roundy    = (flags & SFD_PTFLAG_ROUND_IN_Y) > 0;
	    pt->dontinterpolate = (flags & SFD_PTFLAG_INTERPOLATE_NEVER) > 0;
	    if ( pt->prev!=NULL )
		pt->prev->acceptableextrema = (flags & SFD_PTFLAG_PREV_EXTREMA_MARKED_ACCEPTABLE) > 0;
	    else
		lastacceptable = (flags & SFD_PTFLAG_PREV_EXTREMA_MARKED_ACCEPTABLE) > 0;
	    if ( flags&0x80 )
		pt->ttfindex = 0xffff;
	    else
		pt->ttfindex = ttfindex++;
	    pt->nextcpindex = 0xfffe;
	    ch = nlgetc(sfd);
	    if ( ch=='x' ) {
		pt->hintmask = chunkalloc(sizeof(HintMask));
		SFDGetHintMask(sfd,pt->hintmask);
	    } else if ( ch!=',' )
		ungetc(ch,sfd);
	    else {
		ch = nlgetc(sfd);
		if ( ch==',' )
		    pt->ttfindex = 0xfffe;
		else {
		    ungetc(ch,sfd);
		    getint(sfd,&tmp);
		    pt->ttfindex = tmp;
		    nlgetc(sfd);	/* skip comma */
		    if ( tmp!=-1 )
			ttfindex = tmp+1;
		}
		ch = nlgetc(sfd);
		if ( ch=='\r' || ch=='\n' )
		    ungetc(ch,sfd);
		else {
		    ungetc(ch,sfd);
		    getint(sfd,&tmp);
		    pt->nextcpindex = tmp;
		    if ( tmp!=-1 )
			ttfindex = tmp+1;
		}
	    }
	} else
	    flags = 0;
    }
    if ( cur!=NULL && !(flags & SFD_PTFLAG_FORCE_OPEN_PATH) )
	SFDCloseCheck(cur,order2);
    if ( lastacceptable && cur->last->prev!=NULL )
	cur->last->prev->acceptableextrema = true;
    getname(sfd,tok);
return( head );
}