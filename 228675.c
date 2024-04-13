static enum uni_interp SFDGetUniInterp(FILE *sfd, char *tok, SplineFont *sf) {
    int uniinterp = ui_none;
    int i;

    geteol(sfd,tok);
    for ( i=0; unicode_interp_names[i]!=NULL; ++i )
	if ( strcmp(tok,unicode_interp_names[i])==0 ) {
	    uniinterp = i;
    break;
	}
    /* These values are now handled by namelists */
    if ( uniinterp == ui_adobe ) {
	sf->for_new_glyphs = NameListByName("AGL with PUA");
	uniinterp = ui_none;
    } else if ( uniinterp == ui_greek ) {
	sf->for_new_glyphs = NameListByName("Greek small caps");
	uniinterp = ui_none;
    } else if ( uniinterp == ui_ams ) {
	sf->for_new_glyphs = NameListByName("AMS Names");
	uniinterp = ui_none;
    }

return( uniinterp );
}