static uint8 *image2rle(struct _GImage *img, int *len) {
    int max = img->height*img->bytes_per_line;
    uint8 *rle, *pt, *end;
    int cnt, set;
    int i,j,k;

    *len = 0;
    if ( img->image_type!=it_mono || img->bytes_per_line<5 )
return( NULL );
    rle = calloc(max,sizeof(uint8)), pt = rle, end=rle+max-3;

    for ( i=0; i<img->height; ++i ) {
	if ( i!=0 ) {
	    if ( memcmp(img->data+i*img->bytes_per_line,
			img->data+(i-1)*img->bytes_per_line, img->bytes_per_line)== 0 ) {
		for ( k=1; k<img->height-i; ++k ) {
		    if ( memcmp(img->data+(i+k)*img->bytes_per_line,
				img->data+i*img->bytes_per_line, img->bytes_per_line)!= 0 )
		break;
		}
		i+=k;
		while ( k>0 ) {
		    if ( pt>end ) {
			free(rle);
return( NULL );
		    }
		    *pt++ = 255;
		    *pt++ = 0;
		    *pt++ = k>254 ? 254 : k;
		    k -= 254;
		}
		if ( i>=img->height )
    break;
	    }
	}

	set=1; cnt=0; j=0;
	while ( j<img->width ) {
	    for ( k=j; k<img->width; ++k ) {
		if (( set && !(img->data[i*img->bytes_per_line+(k>>3)]&(0x80>>(k&7))) ) ||
		    ( !set && (img->data[i*img->bytes_per_line+(k>>3)]&(0x80>>(k&7))) ))
	    break;
	    }
	    cnt = k-j;
	    j=k;
	    do {
		if ( pt>=end ) {
		    free(rle);
return( NULL );
		}
		if ( cnt<=254 )
		    *pt++ = cnt;
		else {
		    *pt++ = 255;
		    if ( cnt>65535 ) {
			*pt++ = 255;
			*pt++ = 255;
			*pt++ = 0;		/* nothing of the other color, we've still got more of this one */
		    } else {
			*pt++ = cnt>>8;
			*pt++ = cnt&0xff;
		    }
		}
		cnt -= 65535;
	    } while ( cnt>0 );
	    set = 1-set;
	}
    }
    *len = pt-rle;
return( rle );
}