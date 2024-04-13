static void SFDDumpHintList(FILE *sfd,const char *key, StemInfo *h) {
    HintInstance *hi;

    if ( h==NULL )
return;
    fprintf(sfd, "%s", key );
    for ( ; h!=NULL; h=h->next ) {
	fprintf(sfd, "%g %g", (double) h->start,(double) h->width );
	if ( h->ghost ) putc('G',sfd);
	if ( h->where!=NULL ) {
	    putc('<',sfd);
	    for ( hi=h->where; hi!=NULL; hi=hi->next )
		fprintf(sfd, "%g %g%c", (double) hi->begin, (double) hi->end, hi->next?' ':'>');
	}
	putc(h->next?' ':'\n',sfd);
    }
}