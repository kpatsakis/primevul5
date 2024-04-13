static int SFDDump(FILE *sfd,SplineFont *sf,EncMap *map,EncMap *normal,
	int todir, char *dirname) {
    int i, realcnt;
    BDFFont *bdf;
    int err = false;

    realcnt = sf->glyphcnt;
    if ( sf->subfontcnt!=0 ) {
	for ( i=0; i<sf->subfontcnt; ++i )
	    if ( realcnt<sf->subfonts[i]->glyphcnt )
		realcnt = sf->subfonts[i]->glyphcnt;
    }
    for ( i=0, bdf = sf->bitmaps; bdf!=NULL; bdf=bdf->next, ++i );
    ff_progress_start_indicator(10,_("Saving..."),_("Saving Spline Font Database"),_("Saving Outlines"),
	    realcnt,i+1);
    ff_progress_enable_stop(false);
#ifndef _NO_LIBPNG
    double version = 3.2;
    if (!WritePNGInSFD) version = 3.1;
#else
    double version = 3.1;
#endif
    if (!UndoRedoLimitToSave && version == 3.1) {
        version = 3.0;
    } 
    fprintf(sfd, "SplineFontDB: %.1f\n", version );
    if ( sf->mm != NULL )
	err = SFD_MMDump(sfd,sf->mm->normal,map,normal,todir,dirname);
    else
	err = SFD_Dump(sfd,sf,map,normal,todir,dirname);
    ff_progress_end_indicator();
return( err );
}