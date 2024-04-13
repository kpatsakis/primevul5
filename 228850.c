static void SFDDumpImage(FILE *sfd,ImageList *img) {
    GImage *image = img->image;
    struct _GImage *base = image->list_len==0?image->u.image:image->u.images[0];
    struct enc85 enc;
    int rlelen;
    uint8 *rle;
    int i;

    rle = image2rle(base,&rlelen);
    fprintf(sfd, "Image: %d %d %d %d %d %x %g %g %g %g %d\n",
	    (int) base->width, (int) base->height, base->image_type,
	    (int) (base->image_type==it_true?3*base->width:base->bytes_per_line),
	    base->clut==NULL?0:base->clut->clut_len,(int) base->trans,
	    (double) img->xoff, (double) img->yoff, (double) img->xscale, (double) img->yscale, rlelen );
    memset(&enc,'\0',sizeof(enc));
    enc.sfd = sfd;
    if ( base->clut!=NULL ) {
	for ( i=0; i<base->clut->clut_len; ++i ) {
	    SFDEnc85(&enc,base->clut->clut[i]>>16);
	    SFDEnc85(&enc,(base->clut->clut[i]>>8)&0xff);
	    SFDEnc85(&enc,base->clut->clut[i]&0xff);
	}
    }
    if ( rle!=NULL ) {
	uint8 *pt=rle, *end=rle+rlelen;
	while ( pt<end )
	    SFDEnc85(&enc,*pt++);
	free( rle );
    } else {
	for ( i=0; i<base->height; ++i ) {
	    if ( base->image_type==it_rgba ) {
		uint32 *ipt = (uint32 *) (base->data + i*base->bytes_per_line);
		uint32 *iend = (uint32 *) (base->data + (i+1)*base->bytes_per_line);
		while ( ipt<iend ) {
		    SFDEnc85(&enc,*ipt>>24);
		    SFDEnc85(&enc,(*ipt>>16)&0xff);
		    SFDEnc85(&enc,(*ipt>>8)&0xff);
		    SFDEnc85(&enc,*ipt&0xff);
		    ++ipt;
		}
	    } else if ( base->image_type==it_true ) {
		int *ipt = (int *) (base->data + i*base->bytes_per_line);
		int *iend = (int *) (base->data + (i+1)*base->bytes_per_line);
		while ( ipt<iend ) {
		    SFDEnc85(&enc,*ipt>>16);
		    SFDEnc85(&enc,(*ipt>>8)&0xff);
		    SFDEnc85(&enc,*ipt&0xff);
		    ++ipt;
		}
	    } else {
		uint8 *pt = (uint8 *) (base->data + i*base->bytes_per_line);
		uint8 *end = (uint8 *) (base->data + (i+1)*base->bytes_per_line);
		while ( pt<end ) {
		    SFDEnc85(&enc,*pt);
		    ++pt;
		}
	    }
	}
    }
    SFDEnc85EndEnc(&enc);
    fprintf(sfd,"\nEndImage\n" );
}