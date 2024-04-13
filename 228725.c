static void SFDPickleMe(FILE *sfd,void *python_data, int python_data_has_lists) {
    char *string, *pt;

#ifdef _NO_PYTHON
    string = (char *) python_data;
#else
    string = PyFF_PickleMeToString(python_data);
#endif
    if ( string==NULL )
return;
    if (python_data_has_lists)
    fprintf( sfd, "PickledDataWithLists: \"" );
    else
    fprintf( sfd, "PickledData: \"" );
    for ( pt=string; *pt; ++pt ) {
	if ( *pt=='\\' || *pt=='"' )
	    putc('\\',sfd);
	putc(*pt,sfd);
    }
    fprintf( sfd, "\"\n");
#ifndef _NO_PYTHON
    free(string);
#endif
}