static void SFDDumpRefs(FILE *sfd,RefChar *refs, int *newgids) {
    RefChar *ref;

    for ( ref=refs; ref!=NULL; ref=ref->next ) if ( ref->sc!=NULL ) {
	fprintf(sfd, "Refer: %d %d %c %g %g %g %g %g %g %d",
		    newgids!=NULL ? newgids[ref->sc->orig_pos]:ref->sc->orig_pos,
		    ref->sc->unicodeenc,
		    ref->selected?'S':'N',
		    (double) ref->transform[0], (double) ref->transform[1], (double) ref->transform[2],
		    (double) ref->transform[3], (double) ref->transform[4], (double) ref->transform[5],
		    ref->use_my_metrics|(ref->round_translation_to_grid<<1)|
		     (ref->point_match<<2));
	if ( ref->point_match ) {
	    fprintf(sfd, " %d %d", ref->match_pt_base, ref->match_pt_ref );
	    if ( ref->point_match_out_of_date )
		fprintf( sfd, " O" );
	}
	putc('\n',sfd);
    }
}