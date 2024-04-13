void SFDDumpMacFeat(FILE *sfd,MacFeat *mf) {
    struct macsetting *ms;

    if ( mf==NULL )
return;

    while ( mf!=NULL ) {
	if ( mf->featname!=NULL ) {
	    fprintf( sfd, "MacFeat: %d %d %d\n", mf->feature, mf->ismutex, mf->default_setting );
	    SFDDumpMacName(sfd,mf->featname);
	    for ( ms=mf->settings; ms!=NULL; ms=ms->next ) {
		if ( ms->setname!=NULL ) {
		    fprintf( sfd, "MacSetting: %d\n", ms->setting );
		    SFDDumpMacName(sfd,ms->setname);
		}
	    }
	}
	mf = mf->next;
    }
    fprintf( sfd,"EndMacFeatures\n" );
}