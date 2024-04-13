static int SFDGetBitmapChar(FILE *sfd,BDFFont *bdf) {
    BDFChar *bfc;
    struct enc85 dec;
    int i, enc, orig;
    int width,xmax,xmin,ymax,ymin, vwidth=-1;
    EncMap *map;
    int ch;

    map = bdf->sf->map;

    if ( getint(sfd,&orig)!=1 || orig<0 )
return( 0 );
    if ( getint(sfd,&enc)!=1 )
return( 0 );
    if ( getint(sfd,&width)!=1 )
return( 0 );
    if ( getint(sfd,&xmin)!=1 )
return( 0 );
    if ( getint(sfd,&xmax)!=1 )
return( 0 );
    if ( getint(sfd,&ymin)!=1 )
return( 0 );
    while ( (ch=nlgetc(sfd))==' ');
    ungetc(ch,sfd);
    if ( ch=='\n' || ch=='\r' || getint(sfd,&ymax)!=1 ) {
	/* Old style format, no orig_pos given, shift everything by 1 */
	ymax = ymin;
	ymin = xmax;
	xmax = xmin;
	xmin = width;
	width = enc;
	enc = orig;
	orig = map->map[enc];
    } else {
	while ( (ch=nlgetc(sfd))==' ');
	ungetc(ch,sfd);
	if ( ch!='\n' && ch!='\r' )
	    getint(sfd,&vwidth);
    }
    if ( enc<0 ||xmax<xmin || ymax<ymin )
return( 0 );

    bfc = chunkalloc(sizeof(BDFChar));
    if (bfc == NULL)
        return 0;

    if ( orig==-1 ) {
	bfc->sc = SFMakeChar(bdf->sf,map,enc);
	orig = bfc->sc->orig_pos;
    }

    bfc->orig_pos = orig;
    bfc->width = width;
    bfc->ymax = ymax; bfc->ymin = ymin;
    bfc->xmax = xmax; bfc->xmin = xmin;
    bdf->glyphs[orig] = bfc;
    bfc->sc = bdf->sf->glyphs[orig];
    bfc->vwidth = vwidth!=-1 ? vwidth :
	    rint(bfc->sc->vwidth*bdf->pixelsize / (real) (bdf->sf->ascent+bdf->sf->descent));
    if ( bdf->clut==NULL ) {
	bfc->bytes_per_line = (bfc->xmax-bfc->xmin)/8 +1;
	bfc->depth = 1;
    } else {
	bfc->bytes_per_line = bfc->xmax-bfc->xmin +1;
	bfc->byte_data = true;
	bfc->depth = bdf->clut->clut_len==4 ? 2 : bdf->clut->clut_len==16 ? 4 : 8;
    }
    bfc->bitmap = calloc((bfc->ymax-bfc->ymin+1)*bfc->bytes_per_line,sizeof(uint8));

    memset(&dec,'\0', sizeof(dec)); dec.pos = -1;
    dec.sfd = sfd;
    for ( i=0; i<=bfc->ymax-bfc->ymin; ++i ) {
	uint8 *pt = (uint8 *) (bfc->bitmap + i*bfc->bytes_per_line);
	uint8 *end = (uint8 *) (bfc->bitmap + (i+1)*bfc->bytes_per_line);
	while ( pt<end ) {
	    *pt++ = Dec85(&dec);
	}
    }
    if ( bfc->sc==NULL ) {
	bdf->glyphs[bfc->orig_pos] = NULL;
	BDFCharFree(bfc);
    }
/* This fixes a bug: We didn't set "widthset" on splinechars when reading in */
/*  winfonts. We should set it now on any bitmaps worth outputting to make up*/
/*  for that. Eventually we should have good sfd files and can remove this */
    else if ( bfc->sc->width!=bdf->sf->ascent + bdf->sf->descent )
	bfc->sc->widthset = true;
return( 1 );
}