static ImageList *SFDGetImage(FILE *sfd) {
    /* We've read the image token */
    int width, height, image_type, bpl, clutlen, rlelen;
    uint32 trans;
    struct _GImage *base;
    GImage *image;
    ImageList *img;
    struct enc85 dec;
    int i, ch;

    memset(&dec,'\0', sizeof(dec)); dec.pos = -1;
    dec.sfd = sfd;

    getint(sfd,&width);
    getint(sfd,&height);
    getint(sfd,&image_type);
    getint(sfd,&bpl);
    getint(sfd,&clutlen);
    gethex(sfd,&trans);
    image = GImageCreate(image_type,width,height);
    base = image->list_len==0?image->u.image:image->u.images[0];
    img = calloc(1,sizeof(ImageList));
    img->image = image;
    getreal(sfd,&img->xoff);
    getreal(sfd,&img->yoff);
    getreal(sfd,&img->xscale);
    getreal(sfd,&img->yscale);
    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
    ungetc(ch,sfd);
    rlelen = 0;
    if ( isdigit(ch))
	getint(sfd,&rlelen);
    base->trans = trans;
    if ( clutlen!=0 ) {
	if ( base->clut==NULL )
	    base->clut = calloc(1,sizeof(GClut));
	base->clut->clut_len = clutlen;
	base->clut->trans_index = trans;
	for ( i=0;i<clutlen; ++i ) {
	    int r,g,b;
	    r = Dec85(&dec);
	    g = Dec85(&dec);
	    b = Dec85(&dec);
	    base->clut->clut[i] = (r<<16)|(g<<8)|b;
	}
    }
    if ( rlelen!=0 ) {
	rle2image(&dec,rlelen,base);
    } else {
	for ( i=0; i<height; ++i ) {
	    if ( image_type==it_rgba ) {
		uint32 *ipt = (uint32 *) (base->data + i*base->bytes_per_line);
		uint32 *iend = (uint32 *) (base->data + (i+1)*base->bytes_per_line);
		int r,g,b, a;
		while ( ipt<iend ) {
		    a = Dec85(&dec);
		    r = Dec85(&dec);
		    g = Dec85(&dec);
		    b = Dec85(&dec);
		    *ipt++ = (a<<24)|(r<<16)|(g<<8)|b;
		}
	    } else if ( image_type==it_true ) {
		int *ipt = (int *) (base->data + i*base->bytes_per_line);
		int *iend = (int *) (base->data + (i+1)*base->bytes_per_line);
		int r,g,b;
		while ( ipt<iend ) {
		    r = Dec85(&dec);
		    g = Dec85(&dec);
		    b = Dec85(&dec);
		    *ipt++ = (r<<16)|(g<<8)|b;
		}
	    } else {
		uint8 *pt = (uint8 *) (base->data + i*base->bytes_per_line);
		uint8 *end = (uint8 *) (base->data + (i+1)*base->bytes_per_line);
		while ( pt<end ) {
		    *pt++ = Dec85(&dec);
		}
	    }
	}
    }
    img->bb.minx = img->xoff; img->bb.maxy = img->yoff;
    img->bb.maxx = img->xoff + GImageGetWidth(img->image)*img->xscale;
    img->bb.miny = img->yoff - GImageGetHeight(img->image)*img->yscale;
    /* In old sfd files I failed to recognize bitmap pngs as bitmap, so put */
    /*  in a little check here that converts things which should be bitmap to */
    /*  bitmap */ /* Eventually it can be removed as all old sfd files get */
    /*  converted. 22/10/2002 */
    if ( base->image_type==it_index && base->clut!=NULL && base->clut->clut_len==2 )
	img->image = ImageAlterClut(img->image);
return( img );
}