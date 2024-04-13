static HintInstance *SFDReadHintInstances(FILE *sfd, StemInfo *stem) {
    HintInstance *head=NULL, *last=NULL, *cur;
    real begin, end;
    int ch;

    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
    if ( ch=='G' && stem != NULL ) {
	stem->ghost = true;
	while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
    }
    if ( ch!='<' ) {
	ungetc(ch,sfd);
return(NULL);
    }
    while ( getreal(sfd,&begin)==1 && getreal(sfd,&end)) {
	cur = chunkalloc(sizeof(HintInstance));
	cur->begin = begin;
	cur->end = end;
	if ( head == NULL )
	    head = cur;
	else
	    last->next = cur;
	last = cur;
    }
    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' );
    if ( ch!='>' )
	ungetc(ch,sfd);
return( head );
}