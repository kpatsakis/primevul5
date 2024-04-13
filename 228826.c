static void SFDDumpGuidelines(FILE *sfd, GuidelineSet *gl) {
    if (gl==NULL) {
	return;
    }

    for ( ; gl!=NULL; gl=gl->next )
    {
	fprintf( sfd, "Guideline: " );
	SFDDumpUTF7Str(sfd,gl->name);
	putc(' ',sfd);
	SFDDumpUTF7Str(sfd,gl->identifier);
	putc(' ',sfd);
	fprintf( sfd, "%g %g %g %u %d",
		(double) gl->point.x, (double) gl->point.y,
		(double) gl->angle, gl->color, gl->flags);
	putc('\n',sfd);
    }
}