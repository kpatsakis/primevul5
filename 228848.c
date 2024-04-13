SplineFont *SFRecoverFile(char *autosavename,int inquire,int *state) {
    FILE *asfd;
    SplineFont *ret;
    char tok[1025];

    if (!inquire) {
        *state = 1; //Default to recover all
    }
    if (!ask_about_file(autosavename, state, &asfd)) {
return( NULL );
    }
    locale_t tmplocale; locale_t oldlocale; // Declare temporary locale storage.
    switch_to_c_locale(&tmplocale, &oldlocale); // Switch to the C locale temporarily and cache the old locale.
    ret = SlurpRecovery(asfd,tok,sizeof(tok));
    if ( ret==NULL ) {
	const char *buts[3];
	buts[0] = "_Forget It"; buts[1] = "_Try Again"; buts[2] = NULL;
	if ( ff_ask(_("Recovery Failed"),(const char **) buts,0,1,_("Automagic recovery of changes to %.80s failed.\nShould FontForge try again to recover next time you start it?"),tok)==0 )
	    unlink(autosavename);
    }
    switch_to_old_locale(&tmplocale, &oldlocale); // Switch to the cached locale.
    fclose(asfd);
    if ( ret )
	ret->autosavename = copy(autosavename);
return( ret );
}