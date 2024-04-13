static void SFDDumpDHintList( FILE *sfd,const char *key, DStemInfo *d ) {
    HintInstance *hi;

    if ( d==NULL )
return;
    fprintf(sfd, "%s", key );
    for ( ; d!=NULL; d=d->next ) {
	fprintf(sfd, "%g %g %g %g %g %g",
		(double) d->left.x, (double) d->left.y,
		(double) d->right.x, (double) d->right.y,
		(double) d->unit.x, (double) d->unit.y );
	if ( d->where!=NULL ) {
	    putc('<',sfd);
	    for ( hi=d->where; hi!=NULL; hi=hi->next )
		fprintf(sfd, "%g %g%c", (double) hi->begin, (double) hi->end, hi->next?' ':'>');
	}
	putc(d->next?' ':'\n',sfd);
    }
}