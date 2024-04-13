void SFD_DumpPST( FILE *sfd, SplineChar *sc ) {
    PST *pst;

    for ( pst=sc->possub; pst!=NULL; pst=pst->next ) {
	if (( pst->subtable==NULL && pst->type!=pst_lcaret) || pst->type==pst_null )
	    /* Skip it */;
	else {
	    static const char *keywords[] = { "Null:", "Position2:", "PairPos2:",
		    "Substitution2:",
		    "AlternateSubs2:", "MultipleSubs2:", "Ligature2:",
		    "LCarets2:", NULL };
	    fprintf( sfd, "%s ", keywords[pst->type] );
	    if ( pst->subtable!=NULL ) {
		SFDDumpUTF7Str(sfd,pst->subtable->subtable_name);
		putc(' ',sfd);
	    }
	    if ( pst->type==pst_position ) {
		fprintf( sfd, "dx=%d dy=%d dh=%d dv=%d",
			pst->u.pos.xoff, pst->u.pos.yoff,
			pst->u.pos.h_adv_off, pst->u.pos.v_adv_off);
		SFDDumpValDevTab(sfd,pst->u.pos.adjust);
		putc('\n',sfd);
	    } else if ( pst->type==pst_pair ) {
		fprintf( sfd, "%s dx=%d dy=%d dh=%d dv=%d",
			pst->u.pair.paired,
			pst->u.pair.vr[0].xoff, pst->u.pair.vr[0].yoff,
			pst->u.pair.vr[0].h_adv_off, pst->u.pair.vr[0].v_adv_off );
		SFDDumpValDevTab(sfd,pst->u.pair.vr[0].adjust);
		fprintf( sfd, " dx=%d dy=%d dh=%d dv=%d",
			pst->u.pair.vr[1].xoff, pst->u.pair.vr[1].yoff,
			pst->u.pair.vr[1].h_adv_off, pst->u.pair.vr[1].v_adv_off);
		SFDDumpValDevTab(sfd,pst->u.pair.vr[1].adjust);
		putc('\n',sfd);
	    } else if ( pst->type==pst_lcaret ) {
		int i;
		fprintf( sfd, "%d ", pst->u.lcaret.cnt );
		for ( i=0; i<pst->u.lcaret.cnt; ++i ) {
		    fprintf( sfd, "%d", pst->u.lcaret.carets[i] );
                    if ( i<pst->u.lcaret.cnt-1 ) putc(' ',sfd);
                }
		fprintf( sfd, "\n" );
	    } else
		fprintf( sfd, "%s\n", pst->u.lig.components );
	}
    }
}