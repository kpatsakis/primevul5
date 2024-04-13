static void SFDFixupBitmapRefs( BDFFont *bdf ) {
    BDFChar *bc, *rbc;
    BDFRefChar *head, *next, *prev;
    int i;

    for ( i=0; i<bdf->glyphcnt; i++ ) if (( bc = bdf->glyphs[i] ) != NULL ) {
	prev = NULL;
	for ( head = bc->refs; head != NULL; head = next ) {
	    next = head->next;
	    if (( rbc = bdf->glyphs[head->gid] ) != NULL ) {
		head->bdfc = rbc;
		BCMakeDependent( bc,rbc );
		prev = head;
	    } else {
		LogError(_("Glyph %d in bitmap strike %d pixels refers to a missing glyph (%d)"),
		    bc->orig_pos, bdf->pixelsize, head->gid );
		if ( prev == NULL ) bc->refs = next;
		else prev->next = next;
	    }
	}
    }

}