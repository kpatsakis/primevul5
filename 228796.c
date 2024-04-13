static void SFDDumpSplineSet(FILE *sfd, SplineSet *spl, int want_order2) {
    SplinePoint *first, *sp;
    // If there's no spline structure there should just be a single point,
    // which is compatible with either order and therefore want_order2
    int order2 = spl->first->next!=NULL ? spl->first->next->order2 : want_order2;
    int reduce = (want_order2 && !order2);
    if (order2 && !want_order2)
	IError("Asked for cubic when had quadratic");
    SplineSet *nspl;

    for ( ; spl!=NULL; spl=spl->next ) {
	if (reduce) {
	    nspl = SSttfApprox(spl);
	    order2 = true;
	} else {
	    nspl = spl;
	}
	first = NULL;
	for ( sp = nspl->first; ; sp=sp->next->to ) {
#ifndef FONTFORGE_CONFIG_USE_DOUBLE
	    if ( first==NULL )
		fprintf( sfd, "%g %g m ", (double) sp->me.x, (double) sp->me.y );
	    else if ( sp->prev->islinear && sp->noprevcp )		/* Don't use known linear here. save control points if there are any */
		fprintf( sfd, " %g %g l ", (double) sp->me.x, (double) sp->me.y );
	    else
		fprintf( sfd, " %g %g %g %g %g %g c ",
			(double) sp->prev->from->nextcp.x, (double) sp->prev->from->nextcp.y,
			(double) sp->prevcp.x, (double) sp->prevcp.y,
			(double) sp->me.x, (double) sp->me.y );
#else
	    if ( first==NULL )
		fprintf( sfd, "%.12g %.12g m ", (double) sp->me.x, (double) sp->me.y );
	    else if ( sp->prev->islinear && sp->noprevcp )		/* Don't use known linear here. save control points if there are any */
		fprintf( sfd, " %.12g %.12g l ", (double) sp->me.x, (double) sp->me.y );
	    else
		fprintf( sfd, " %.12g %.12g %.12g %.12g %.12g %.12g c ",
			(double) sp->prev->from->nextcp.x, (double) sp->prev->from->nextcp.y,
			(double) sp->prevcp.x, (double) sp->prevcp.y,
			(double) sp->me.x, (double) sp->me.y );
#endif
	    int ptflags = 0;
	    ptflags = sp->pointtype|(sp->selected<<2)|
		(sp->nextcpdef<<3)|(sp->prevcpdef<<4)|
		(sp->roundx<<5)|(sp->roundy<<6)|
		(sp->ttfindex==0xffff?(1<<7):0)|
		(sp->dontinterpolate<<8)|
		((sp->prev && sp->prev->acceptableextrema)<<9);

	    // Last point in the splineset, and we are an open path.
	    if( !sp->next
		&& spl->first && !spl->first->prev )
	    {
		ptflags |= SFD_PTFLAG_FORCE_OPEN_PATH;
	    }


	    fprintf(sfd, "%d", ptflags );
	    if ( order2 ) {
		if ( sp->ttfindex!=0xfffe && sp->nextcpindex!=0xfffe ) {
		    putc(',',sfd);
		    if ( sp->ttfindex==0xffff )
			fprintf(sfd,"-1");
		    else if ( sp->ttfindex!=0xfffe )
			fprintf(sfd,"%d",sp->ttfindex);
		    if ( sp->nextcpindex==0xffff )
			fprintf(sfd,",-1");
		    else if ( sp->nextcpindex!=0xfffe )
			fprintf(sfd,",%d",sp->nextcpindex);
		}
	    } else {
		if ( sp->hintmask!=NULL ) {
		    putc('x',sfd);
		    SFDDumpHintMask(sfd, sp->hintmask);
		}
	    }
	    putc('\n',sfd);
	    if (sp->name != NULL) {
		fputs("NamedP: ", sfd);
		SFDDumpUTF7Str(sfd, sp->name);
		putc('\n', sfd);
	    }
	    if ( sp==first )
	break;
	    if ( first==NULL ) first = sp;
	    if ( sp->next==NULL )
	break;
	}
	if ( spl->spiro_cnt!=0 ) {
	    int i;
	    fprintf( sfd, "  Spiro\n" );
	    for ( i=0; i<spl->spiro_cnt; ++i ) {
		fprintf( sfd, "    %g %g %c\n", spl->spiros[i].x, spl->spiros[i].y,
			    spl->spiros[i].ty&0x7f);
	    }
	    fprintf( sfd, "  EndSpiro\n" );
	}
	if ( spl->contour_name!=NULL ) {
	    fprintf( sfd, "  Named: " );
	    SFDDumpUTF7Str(sfd,spl->contour_name);
	    putc('\n',sfd);
	}
	if ( spl->is_clip_path ) {
	    fprintf( sfd, "  PathFlags: %d\n", spl->is_clip_path );
	}
	if ( spl->start_offset ) {
	    fprintf( sfd, "  PathStart: %d\n", spl->start_offset );
	}
    if (reduce) SplinePointListFree(nspl);
    }
    fprintf( sfd, "EndSplineSet\n" );
}