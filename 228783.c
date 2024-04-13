char* SFDMoveToNextStartChar( FILE* sfd ) {
    char ret[2000];

    memset( ret, '\0', 2000 );
    char* line = 0;
    while((line = getquotedeol( sfd ))) {
	if( !strnmatch( line, "StartChar:", strlen( "StartChar:" ))) {
	    // FIXME: use the getname()/SFDReadUTF7Str() combo
	    // from SFDGetChar
	    int len = strlen("StartChar:");
	    while( line[len] && line[len] == ' ' )
		len++;
	    strcpy( ret, line+len );
	    free(line);
	    return copy(ret);
	}
	free(line);
	if(feof( sfd ))
	    break;

    }
    return 0;
}