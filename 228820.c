void SFDDumpUndo(FILE *sfd,SplineChar *sc,Undoes *u, const char* keyPrefix, int idx ) {
    fprintf(sfd, "%sOperation\n",      keyPrefix );
    fprintf(sfd, "Index: %d\n",        idx );
    fprintf(sfd, "Type: %d\n",         u->undotype );
    fprintf(sfd, "WasModified: %d\n",  u->was_modified );
    fprintf(sfd, "WasOrder2: %d\n",    u->was_order2 );
    if( u->layer != UNDO_LAYER_UNKNOWN )
    {
	fprintf(sfd, "Layer: %d\n",    u->layer );
    }

    switch( u->undotype )
    {
        case ut_tstate:
        case ut_state:
            fprintf(sfd, "Width: %d\n",           u->u.state.width );
            fprintf(sfd, "VWidth: %d\n",          u->u.state.vwidth );
            fprintf(sfd, "LBearingChange: %d\n",  u->u.state.lbearingchange );
            fprintf(sfd, "UnicodeEnc: %d\n",      u->u.state.unicodeenc );
            if( u->u.state.charname )
                fprintf(sfd, "Charname: \"%s\"\n", u->u.state.charname );
            if( u->u.state.comment )
                fprintf(sfd, "Comment: \"%s\"\n", u->u.state.comment );
            if( u->u.state.refs ) {
                SFDDumpRefs( sfd, u->u.state.refs, 0 );
            }
	    if( u->u.state.images ) {
#ifndef _NO_LIBPNG
                if (WritePNGInSFD)
                    SFDDumpImagePNG( sfd, u->u.state.images );
                else
#endif
                    SFDDumpImage( sfd, u->u.state.images );
            }
            fprintf(sfd, "InstructionsLength: %d\n", u->u.state.instrs_len );
            if( u->u.state.anchor ) {
                SFDDumpAnchorPoints( sfd, u->u.state.anchor );
            }
	    if( u->u.state.splines ) {
                fprintf(sfd, "SplineSet\n" );
                SFDDumpSplineSet( sfd, u->u.state.splines, u->was_order2 );
            }
            break;

        case ut_statehint:
        {
            SplineChar* tsc = 0;
            tsc = SplineCharCopy( sc, 0, 0 );
            ExtractHints( tsc, u->u.state.hints, 1 );
            SFDDumpHintList(  sfd, "HStem: ",  tsc->hstem);
            SFDDumpHintList(  sfd, "VStem: ",  tsc->vstem);
            SFDDumpDHintList( sfd, "DStem2: ", tsc->dstem);
            SplineCharFree( tsc );

	    if( u->u.state.instrs_len )
                SFDDumpTtfInstrsExplicit( sfd, u->u.state.instrs, u->u.state.instrs_len );
            break;
        }

        case ut_hints:
        {
            SplineChar* tsc = 0;
            tsc = SplineCharCopy( sc, 0, 0 );
            tsc->ttf_instrs = 0;
            ExtractHints( tsc, u->u.state.hints, 1 );
            SFDDumpHintList(  sfd, "HStem: ",  tsc->hstem);
            SFDDumpHintList(  sfd, "VStem: ",  tsc->vstem);
            SFDDumpDHintList( sfd, "DStem2: ", tsc->dstem);
            SplineCharFree( tsc );

            if( u->u.state.instrs_len )
                SFDDumpTtfInstrsExplicit( sfd, u->u.state.instrs, u->u.state.instrs_len );
            if( u->copied_from && u->copied_from->fullname )
                fprintf(sfd, "CopiedFrom: %s\n", u->copied_from->fullname );
            break;
        }

        case ut_width:
        case ut_vwidth:
        {
            fprintf(sfd, "Width: %d\n", u->u.width );
            break;
        }

        default:
        break;
    }

    fprintf(sfd, "End%sOperation\n", keyPrefix );
}