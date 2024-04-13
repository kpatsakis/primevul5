static void SFDFixupUndoRefsUndoList(SplineFont *sf,Undoes *undo) {
    for( ; undo; undo = undo->next ) {
	if( undo->undotype == ut_state && undo->u.state.refs ) {
	    RefChar *ref=NULL;
	    for ( ref=undo->u.state.refs; ref!=NULL; ref=ref->next ) {

		ref->sc = sf->glyphs[ ref->orig_pos ];
	    }
	}
    }
}