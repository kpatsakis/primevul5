int SFDWrite(char *filename,SplineFont *sf,EncMap *map,EncMap *normal,int todir) {
    FILE *sfd;
    int i, gc;
    char *tempfilename = filename;
    int err = false;

    if ( todir ) {
	SFDirClean(filename);
	GFileMkDir(filename, 0755);		/* this will fail if directory already exists. That's ok */
	tempfilename = malloc(strlen(filename)+strlen("/" FONT_PROPS)+1);
	strcpy(tempfilename,filename); strcat(tempfilename,"/" FONT_PROPS);
    }

    sfd = fopen(tempfilename,"w");
    if ( tempfilename!=filename ) free(tempfilename);
    if ( sfd==NULL )
return( 0 );

    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.
    if ( sf->cidmaster!=NULL ) {
	sf=sf->cidmaster;
	gc = 1;
	for ( i=0; i<sf->subfontcnt; ++i )
	    if ( sf->subfonts[i]->glyphcnt > gc )
		gc = sf->subfonts[i]->glyphcnt;
	map = EncMap1to1(gc);
	err = SFDDump(sfd,sf,map,NULL,todir,filename);
	EncMapFree(map);
    } else
	err = SFDDump(sfd,sf,map,normal,todir,filename);
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
    if ( ferror(sfd) ) err = true;
    if ( fclose(sfd) ) err = true;
    if ( todir )
	SFFinalDirClean(filename);
return( !err );
}