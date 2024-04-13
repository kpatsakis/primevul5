static void SFDDumpGlyphVariants(FILE *sfd,struct glyphvariants *gv,const char *name) {
    int i;

    if ( gv==NULL )
return;

    if ( gv->variants!=NULL )
	fprintf( sfd, "GlyphVariants%s: %s\n", name, gv->variants );
    if ( gv->part_cnt!=0 ) {
	if ( gv->italic_correction!=0 ) {
	    fprintf( sfd, "GlyphComposition%sIC: %d", name, gv->italic_correction );
	    if ( gv->italic_adjusts!=NULL ) {
		putc(' ',sfd);
		SFDDumpDeviceTable(sfd,gv->italic_adjusts);
	    }
	    putc('\n',sfd);
	}
	fprintf( sfd, "GlyphComposition%s: %d ", name, gv->part_cnt );
	for ( i=0; i<gv->part_cnt; ++i ) {
	    fprintf( sfd, " %s%%%d,%d,%d,%d", gv->parts[i].component,
		    gv->parts[i].is_extender,
		    gv->parts[i].startConnectorLength,
		    gv->parts[i].endConnectorLength,
		    gv->parts[i].fullAdvance);
	}
	putc('\n',sfd);
    }
}