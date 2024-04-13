static void SFDDumpCharMath(FILE *sfd,SplineChar *sc) {
    if ( sc->italic_correction!=TEX_UNDEF && sc->italic_correction!=0 ) {
	fprintf( sfd, "ItalicCorrection: %d", sc->italic_correction );
	if ( sc->italic_adjusts!=NULL ) {
	    putc(' ',sfd);
	    SFDDumpDeviceTable(sfd,sc->italic_adjusts);
	}
	putc('\n',sfd);
    }
    if ( sc->top_accent_horiz!=TEX_UNDEF ) {
	fprintf( sfd, "TopAccentHorizontal: %d", sc->top_accent_horiz );
	if ( sc->top_accent_adjusts!=NULL ) {
	    putc(' ',sfd);
	    SFDDumpDeviceTable(sfd,sc->top_accent_adjusts);
	}
	putc('\n',sfd);
    }
    if ( sc->is_extended_shape )
	fprintf( sfd, "IsExtendedShape: %d\n", sc->is_extended_shape );
    SFDDumpGlyphVariants(sfd,sc->vert_variants,"Vertical");
    SFDDumpGlyphVariants(sfd,sc->horiz_variants,"Horizontal");
    if ( sc->mathkern!=NULL ) {
	SFDDumpMathVertex(sfd,&sc->mathkern->top_right,"TopRightVertex:");
	SFDDumpMathVertex(sfd,&sc->mathkern->top_left,"TopLeftVertex:");
	SFDDumpMathVertex(sfd,&sc->mathkern->bottom_right,"BottomRightVertex:");
	SFDDumpMathVertex(sfd,&sc->mathkern->bottom_left,"BottomLeftVertex:");
    }
}