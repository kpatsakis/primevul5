static int SFDGetBitmapFont(FILE *sfd,SplineFont *sf,int fromdir,char *dirname) {
    BDFFont *bdf, *prev;
    char tok[2000];
    int pixelsize, ascent, descent, depth=1;
    int ch, enccount;

    if ( getint(sfd,&pixelsize)!=1 || pixelsize<=0 )
return( 0 );
    if ( getint(sfd,&enccount)!=1 || enccount<0 )
return( 0 );
    if ( getint(sfd,&ascent)!=1 || ascent<0 )
return( 0 );
    if ( getint(sfd,&descent)!=1 || descent<0 )
return( 0 );
    if ( getint(sfd,&depth)!=1 )
	depth = 1;	/* old sfds don't have a depth here */
    else if ( depth!=1 && depth!=2 && depth!=4 && depth!=8 )
return( 0 );
    while ( (ch = nlgetc(sfd))==' ' );
    ungetc(ch,sfd);		/* old sfds don't have a foundry */

    bdf = calloc(1,sizeof(BDFFont));
    if (bdf == NULL)
        return 0;

    if ( ch!='\n' && ch!='\r' ) {
	getname(sfd,tok);
	bdf->foundry = copy(tok);
    }
    bdf->pixelsize = pixelsize;
    bdf->ascent = ascent;
    bdf->descent = descent;
    if ( depth!=1 )
	BDFClut(bdf,(1<<(depth/2)));

    if ( sf->bitmaps==NULL )
	sf->bitmaps = bdf;
    else {
	for ( prev=sf->bitmaps; prev->next!=NULL; prev=prev->next );
	prev->next = bdf;
    }
    bdf->sf = sf;
    bdf->glyphcnt = bdf->glyphmax = sf->glyphcnt;
    bdf->glyphs = calloc(bdf->glyphcnt,sizeof(BDFChar *));

    while ( getname(sfd,tok)==1 ) {
	if ( strcmp(tok,"BDFStartProperties:")==0 )
	    SFDGetBitmapProps(sfd,bdf,tok);
	else if ( strcmp(tok,"BDFEndProperties")==0 )
	    /* Do Nothing */;
	else if ( strcmp(tok,"Resolution:")==0 )
	    getint(sfd,&bdf->res);
	else if ( strcmp(tok,"BDFChar:")==0 )
	    SFDGetBitmapChar(sfd,bdf);
	else if ( strcmp(tok,"BDFRefChar:")==0 )
	    SFDGetBitmapReference(sfd,bdf);
	else if ( strcmp(tok,"EndBitmapFont")==0 )
    break;
    }
    if ( fromdir ) {
	DIR *dir;
	struct dirent *ent;
	char *name;

	dir = opendir(dirname);
	if ( dir==NULL )
return( 0 );
	name = malloc(strlen(dirname)+NAME_MAX+3);

	while ( (ent=readdir(dir))!=NULL ) {
	    char *pt = strrchr(ent->d_name,EXT_CHAR);
	    if ( pt==NULL )
		/* Nothing interesting */;
	    else if ( strcmp(pt,BITMAP_EXT)==0 ) {
		FILE *gsfd;
		sprintf(name,"%s/%s", dirname, ent->d_name);
		gsfd = fopen(name,"r");
		if ( gsfd!=NULL ) {
		    if ( getname(gsfd,tok) && strcmp(tok,"BDFChar:")==0)
			SFDGetBitmapChar(gsfd,bdf);
		    fclose(gsfd);
		    ff_progress_next();
		}
	    }
	}
	free(name);
	closedir(dir);
    }
    SFDFixupBitmapRefs( bdf );
return( 1 );
}