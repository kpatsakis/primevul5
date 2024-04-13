static RefChar *SFDGetRef(FILE *sfd, int was_enc) {
    RefChar *rf;
    int temp=0, ch;

    rf = RefCharCreate();
    getint(sfd,&rf->orig_pos);
    rf->encoded = was_enc;
    if ( getint(sfd,&temp))
	rf->unicode_enc = temp;
    while ( isspace(ch=nlgetc(sfd)));
    if ( ch=='S' ) rf->selected = true;
    getreal(sfd,&rf->transform[0]);
    getreal(sfd,&rf->transform[1]);
    getreal(sfd,&rf->transform[2]);
    getreal(sfd,&rf->transform[3]);
    getreal(sfd,&rf->transform[4]);
    getreal(sfd,&rf->transform[5]);
    while ( (ch=nlgetc(sfd))==' ');
    ungetc(ch,sfd);
    if ( isdigit(ch) ) {
	getint(sfd,&temp);
	rf->use_my_metrics = temp&1;
	rf->round_translation_to_grid = (temp&2)?1:0;
	rf->point_match = (temp&4)?1:0;
	if ( rf->point_match ) {
	    getsint(sfd,(int16 *) &rf->match_pt_base);
	    getsint(sfd,(int16 *) &rf->match_pt_ref);
	    while ( (ch=nlgetc(sfd))==' ');
	    if ( ch=='O' )
		rf->point_match_out_of_date = true;
	    else
		ungetc(ch,sfd);
	}
    }
return( rf );
}