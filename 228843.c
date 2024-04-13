static void SFDDumpAnchorPoints(FILE *sfd,AnchorPoint *ap) {
    if (ap==NULL) {
	return;
    }

    for ( ; ap!=NULL; ap=ap->next )
    {
	fprintf( sfd, "AnchorPoint: " );
	SFDDumpUTF7Str(sfd,ap->anchor->name);
	putc(' ',sfd);
	fprintf( sfd, "%g %g %s %d",
		(double) ap->me.x, (double) ap->me.y,
		ap->type==at_centry ? "entry" :
		ap->type==at_cexit ? "exit" :
		ap->type==at_mark ? "mark" :
		ap->type==at_basechar ? "basechar" :
		ap->type==at_baselig ? "baselig" : "basemark",
		ap->lig_index );
	if ( ap->xadjust.corrections!=NULL || ap->yadjust.corrections!=NULL ) {
	    putc(' ',sfd);
	    SFDDumpDeviceTable(sfd,&ap->xadjust);
	    putc(' ',sfd);
	    SFDDumpDeviceTable(sfd,&ap->yadjust);
	} else
	if ( ap->has_ttf_pt )
	    fprintf( sfd, " %d", ap->ttf_pt_index );
	putc('\n',sfd);
    }
}