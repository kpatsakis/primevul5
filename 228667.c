static void SFDGetNameList(FILE *sfd, char *tok, SplineFont *sf) {
    NameList *nl;

    geteol(sfd,tok);
    nl = NameListByName(tok);
    if ( nl==NULL )
	LogError(_("Failed to find NameList: %s"), tok);
    else
	sf->for_new_glyphs = nl;
}