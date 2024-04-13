static void rle2image(struct enc85 *dec,int rlelen,struct _GImage *base) {
    uint8 *pt, *end;
    int r,c,set, cnt, ch, ch2;
    int i;

    r = c = 0; set = 1; pt = base->data; end = pt + base->bytes_per_line*base->height;
    memset(base->data,0xff,end-pt);
    while ( rlelen>0 ) {
	if ( pt>=end ) {
	    IError( "RLE failure\n" );
	    while ( rlelen>0 ) { Dec85(dec); --rlelen; }
    break;
	}
	ch = Dec85(dec);
	--rlelen;
	if ( ch==255 ) {
	    ch2 = Dec85(dec);
	    cnt = (ch2<<8) + Dec85(dec);
	    rlelen -= 2;
	} else
	    cnt = ch;
	if ( ch==255 && ch2==0 && cnt<255 ) {
	    /* Line duplication */
	    for ( i=0; i<cnt && pt<end; ++i ) {
		memcpy(pt,base->data+(r-1)*base->bytes_per_line,base->bytes_per_line);
		++r;
		pt += base->bytes_per_line;
	    }
	    set = 1;
	} else {
	    if ( pt + ((c+cnt)>>3) > end ) {
		IError( "Run length encoded image has been corrupted.\n" );
    break;
	    }
	    if ( !set ) {
		for ( i=0; i<cnt; ++i )
		    pt[(c+i)>>3] &= ((~0x80)>>((c+i)&7));
	    }
	    c += cnt;
	    set = 1-set;
	    if ( c>=base->width ) {
		++r;
		pt += base->bytes_per_line;
		c = 0; set = 1;
	    }
	}
    }
}