static void SFDDumpOtfFeatNames(FILE *sfd, SplineFont *sf) {
    struct otffeatname *fn;
    struct otfname *on;

    for ( fn=sf->feat_names; fn!=NULL; fn=fn->next ) {
	fprintf( sfd, "OtfFeatName: '%c%c%c%c' ",
		fn->tag>>24, fn->tag>>16, fn->tag>>8, fn->tag );
	for ( on=fn->names; on!=NULL; on=on->next ) {
	    fprintf( sfd, "%d ", on->lang );
	    SFDDumpUTF7Str(sfd, on->name);
	    if ( on->next!=NULL ) putc(' ',sfd);
	}
	putc('\n',sfd);
    }
}