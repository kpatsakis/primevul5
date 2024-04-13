static StemInfo *SFDReadHints(FILE *sfd) {
    StemInfo *head=NULL, *last=NULL, *cur;
    real start, width;

    while ( getreal(sfd,&start)==1 && getreal(sfd,&width)) {
	cur = chunkalloc(sizeof(StemInfo));
	cur->start = start;
	cur->width = width;
	cur->where = SFDReadHintInstances(sfd,cur);
	if ( head == NULL )
	    head = cur;
	else
	    last->next = cur;
	last = cur;
    }
return( head );
}