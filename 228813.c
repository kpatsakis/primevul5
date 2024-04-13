static GuidelineSet *SFDReadGuideline(FILE *sfd, GuidelineSet **gll, GuidelineSet *lastgl)
{
    GuidelineSet *gl = chunkalloc(sizeof(GuidelineSet));
    gl->name = SFDReadUTF7Str(sfd);
    gl->identifier = SFDReadUTF7Str(sfd);
    getreal(sfd,&gl->point.x);
    getreal(sfd,&gl->point.y);
    getreal(sfd,&gl->angle);
    getint(sfd,&gl->color);
    getint(sfd,&gl->flags);
    if ( lastgl!=NULL )
	lastgl->next = gl;
    else if (gll)
        *gll = gl;
    return( gl );
}