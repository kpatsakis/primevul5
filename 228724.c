static int SFDDumpBitmapFont(FILE *sfd,BDFFont *bdf,EncMap *encm,int *newgids,
	int todir, char *dirname) {
    int i;
    int err = false;
    BDFChar *bc;
    BDFRefChar *ref;

    ff_progress_next_stage();
    if (bdf->foundry)
        fprintf( sfd, "BitmapFont: %d %d %d %d %d %s\n", bdf->pixelsize, bdf->glyphcnt,
                 bdf->ascent, bdf->descent, BDFDepth(bdf), bdf->foundry );
    else
        fprintf( sfd, "BitmapFont: %d %d %d %d %d\n", bdf->pixelsize, bdf->glyphcnt,
                 bdf->ascent, bdf->descent, BDFDepth(bdf) );
    if ( bdf->prop_cnt>0 ) {
	fprintf( sfd, "BDFStartProperties: %d\n", bdf->prop_cnt );
	for ( i=0; i<bdf->prop_cnt; ++i ) {
	    fprintf(sfd,"%s %d ", bdf->props[i].name, bdf->props[i].type );
	    switch ( bdf->props[i].type&~prt_property ) {
	      case prt_int: case prt_uint:
		fprintf(sfd, "%d\n", bdf->props[i].u.val );
	      break;
	      case prt_string: case prt_atom:
		fprintf(sfd, "\"%s\"\n", bdf->props[i].u.str );
	      break;
	      default:
	      break;
	    }
	}
	fprintf( sfd, "BDFEndProperties\n" );
    }
    if ( bdf->res>20 )
	fprintf( sfd, "Resolution: %d\n", bdf->res );
    for ( i=0; i<bdf->glyphcnt; ++i ) {
	if ( bdf->glyphs[i]!=NULL ) {
	    if ( todir ) {
		char *glyphfile = malloc(strlen(dirname)+2*strlen(bdf->glyphs[i]->sc->name)+20);
		FILE *gsfd;
		appendnames(glyphfile,dirname,"/",bdf->glyphs[i]->sc->name,BITMAP_EXT );
		gsfd = fopen(glyphfile,"w");
		if ( gsfd!=NULL ) {
		    SFDDumpBitmapChar(gsfd,bdf->glyphs[i],encm->backmap[i],newgids);
		    if ( ferror(gsfd)) err = true;
		    if ( fclose(gsfd)) err = true;
		} else
		    err = true;
		free(glyphfile);
	    } else
		SFDDumpBitmapChar(sfd,bdf->glyphs[i],encm->backmap[i],newgids);
	}
	ff_progress_next();
    }
    for ( i=0; i<bdf->glyphcnt; ++i ) if (( bc = bdf->glyphs[i] ) != NULL ) {
    	for ( ref=bc->refs; ref!=NULL; ref=ref->next )
	    fprintf(sfd, "BDFRefChar: %d %d %d %d %c\n",
		newgids!=NULL ? newgids[bc->orig_pos] : bc->orig_pos,
		newgids!=NULL ? newgids[ref->bdfc->orig_pos] : ref->bdfc->orig_pos,
		ref->xoff,ref->yoff,ref->selected?'S':'N' );
    }
    fprintf( sfd, "EndBitmapFont\n" );
return( err );
}