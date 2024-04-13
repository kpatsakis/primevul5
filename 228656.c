static void SFDDumpDesignSize(FILE *sfd, SplineFont *sf) {
    struct otfname *on;

    if ( sf->design_size==0 )
return;

    fprintf( sfd, "DesignSize: %d", sf->design_size );
    if ( sf->fontstyle_id!=0 || sf->fontstyle_name!=NULL ||
	    sf->design_range_bottom!=0 || sf->design_range_top!=0 ) {
	fprintf( sfd, " %d-%d %d ",
		sf->design_range_bottom, sf->design_range_top,
		sf->fontstyle_id );
	for ( on=sf->fontstyle_name; on!=NULL; on=on->next ) {
	    fprintf( sfd, "%d ", on->lang );
	    SFDDumpUTF7Str(sfd, on->name);
	    if ( on->next!=NULL ) putc(' ',sfd);
	}
    }
    putc('\n',sfd);
}