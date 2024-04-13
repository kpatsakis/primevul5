static void *SFDUnPickle(FILE *sfd, int python_data_has_lists) {
    int ch, quoted;
    static int max = 0;
    static char *buf = NULL;
    char *pt, *end;
    int cnt;

    pt = buf; end = buf+max;
    while ( (ch=nlgetc(sfd))!='"' && ch!='\n' && ch!=EOF );
    if ( ch!='"' )
return( NULL );

    quoted = false;
    while ( ((ch=nlgetc(sfd))!='"' || quoted) && ch!=EOF ) {
	if ( !quoted && ch=='\\' )
	    quoted = true;
	else {
	    if ( pt>=end ) {
		cnt = pt-buf;
		buf = realloc(buf,(max+=200)+1);
		pt = buf+cnt;
		end = buf+max;
	    }
	    *pt++ = ch;
	    quoted = false;
	}
    }
    if ( pt==buf )
return( NULL );
    *pt='\0';
#ifdef _NO_PYTHON
return( copy(buf));
#else
return( PyFF_UnPickleMeToObjects(buf));
#endif
    /* buf is a static buffer, I don't free it, I'll reuse it next time */
}