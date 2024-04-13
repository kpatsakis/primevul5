static void SFDDumpPrivate(FILE *sfd,struct psdict *private) {
    int i;
    char *pt;
    /* These guys should all be ascii text */
    fprintf( sfd, "BeginPrivate: %d\n", private->next );
    for ( i=0; i<private->next ; ++i ) {
      fprintf( sfd, "%s %d ", private->keys[i],
	       (int)strlen(private->values[i]));
	for ( pt = private->values[i]; *pt; ++pt )
	    putc(*pt,sfd);
	putc('\n',sfd);
    }
    fprintf( sfd, "EndPrivate\n" );
}