static void SFDGetMinimumDistances(FILE *sfd, SplineChar *sc) {
    SplineSet *ss;
    SplinePoint *sp;
    int pt,i, val, err;
    int ch;
    SplinePoint **mapping=NULL;
    MinimumDistance *last, *md, *mdhead=NULL;

    for ( i=0; i<2; ++i ) {
	pt = 0;
	for ( ss = sc->layers[ly_fore].splines; ss!=NULL; ss=ss->next ) {
	    for ( sp=ss->first; ; ) {
		if ( mapping!=NULL ) mapping[pt] = sp;
		pt++;
		if ( sp->next == NULL )
	    break;
		sp = sp->next->to;
		if ( sp==ss->first )
	    break;
	    }
	}
	if ( mapping==NULL )
	    mapping = calloc(pt,sizeof(SplinePoint *));
    }

    last = NULL;
    for ( ch=nlgetc(sfd); ch!=EOF && ch!='\n'; ch=nlgetc(sfd)) {
	err = false;
	while ( isspace(ch) && ch!='\n' ) ch=nlgetc(sfd);
	if ( ch=='\n' )
    break;
	md = chunkalloc(sizeof(MinimumDistance));
	if ( ch=='x' ) md->x = true;
	getint(sfd,&val);
	if ( val<-1 || val>=pt ) {
	    IError( "Minimum Distance specifies bad point (%d) in sfd file\n", val );
	    err = true;
	} else if ( val!=-1 ) {
	    md->sp1 = mapping[val];
	    md->sp1->dontinterpolate = true;
	}
	ch = nlgetc(sfd);
	if ( ch!=',' ) {
	    IError( "Minimum Distance lacks a comma where expected\n" );
	    err = true;
	}
	getint(sfd,&val);
	if ( val<-1 || val>=pt ) {
	    IError( "Minimum Distance specifies bad point (%d) in sfd file\n", val );
	    err = true;
	} else if ( val!=-1 ) {
	    md->sp2 = mapping[val];
	    md->sp2->dontinterpolate = true;
	}
	if ( !err ) {
	    if ( last!=NULL )
		last->next = md;
	    last = md;
	} else
	    chunkfree(md,sizeof(MinimumDistance));
    }
    free(mapping);

    /* Obsolete concept */
    MinimumDistancesFree(mdhead);
}