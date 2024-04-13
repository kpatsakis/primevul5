static void SFDGetGasp(FILE *sfd,SplineFont *sf) {
    int i;

    getsint(sfd,(int16 *) &sf->gasp_cnt);
    sf->gasp = malloc(sf->gasp_cnt*sizeof(struct gasp));
    for ( i=0; i<sf->gasp_cnt; ++i ) {
	getsint(sfd,(int16 *) &sf->gasp[i].ppem);
	getsint(sfd,(int16 *) &sf->gasp[i].flags);
    }
    getsint(sfd,(int16 *) &sf->gasp_version);
}