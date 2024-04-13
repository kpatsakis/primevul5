static SplineFont *SlurpRecovery(FILE *asfd,char *tok,int sizetok) {
    char *pt; int ch;
    SplineFont *sf;

    ch=nlgetc(asfd);
    ungetc(ch,asfd);
    if ( ch=='B' ) {
	if ( getname(asfd,tok)!=1 )
return(NULL);
	if ( strcmp(tok,"Base:")!=0 )
return(NULL);
	while ( isspace(ch=nlgetc(asfd)) && ch!=EOF && ch!='\n' );
	for ( pt=tok; ch!=EOF && ch!='\n'; ch = nlgetc(asfd) )
	    if ( pt<tok+sizetok-2 )
		*pt++ = ch;
	*pt = '\0';
	sf = LoadSplineFont(tok,0);
    } else {
	sf = SplineFontNew();
	sf->onlybitmaps = false;
	strcpy(tok,"<New File>");
    }
    if ( sf==NULL )
return( NULL );

    if ( !ModSF(asfd,sf)) {
	SplineFontFree(sf);
return( NULL );
    }
return( sf );
}