static void SFDGetOtfFeatName(FILE *sfd,SplineFont *sf) {
    int ch;
    struct otfname *cur;
    struct otffeatname *fn;

    fn = chunkalloc(sizeof(struct otffeatname));
    fn->tag = gettag(sfd);
    for (;;) {
	while ( (ch=nlgetc(sfd))==' ' );
	ungetc(ch,sfd);
	if ( !isdigit(ch))
    break;
	cur = chunkalloc(sizeof(struct otfname));
	cur->next = fn->names;
	fn->names = cur;
	getsint(sfd,(int16 *) &cur->lang);
	cur->name = SFDReadUTF7Str(sfd);
    }
    fn->next = sf->feat_names;
    sf->feat_names = fn;
}