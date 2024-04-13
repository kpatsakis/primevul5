static void SFDGetDesignSize(FILE *sfd,SplineFont *sf) {
    int ch;
    struct otfname *cur;

    getsint(sfd,(int16 *) &sf->design_size);
    while ( (ch=nlgetc(sfd))==' ' );
    ungetc(ch,sfd);
    if ( isdigit(ch)) {
	getsint(sfd,(int16 *) &sf->design_range_bottom);
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!='-' )
	    ungetc(ch,sfd);
	getsint(sfd,(int16 *) &sf->design_range_top);
	getsint(sfd,(int16 *) &sf->fontstyle_id);
	for (;;) {
	    while ( (ch=nlgetc(sfd))==' ' );
	    ungetc(ch,sfd);
	    if ( !isdigit(ch))
	break;
	    cur = chunkalloc(sizeof(struct otfname));
	    cur->next = sf->fontstyle_name;
	    sf->fontstyle_name = cur;
	    getsint(sfd,(int16 *) &cur->lang);
	    cur->name = SFDReadUTF7Str(sfd);
	}
    }
}