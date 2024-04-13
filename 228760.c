static char *SFDParseMMSubroutine(FILE *sfd) {
    char buffer[400], *sofar=calloc(1,1);
    const char *endtok = "EndMMSubroutine";
    int len = 0, blen, first=true;

    while ( fgets(buffer,sizeof(buffer),sfd)!=NULL ) {
	if ( strncmp(buffer,endtok,strlen(endtok))==0 )
    break;
	if ( first ) {
	    first = false;
	    if ( strcmp(buffer,"\n")==0 )
    continue;
	}
	blen = strlen(buffer);
	sofar = realloc(sofar,len+blen+1);
	strcpy(sofar+len,buffer);
	len += blen;
    }
    if ( len>0 && sofar[len-1]=='\n' )
	sofar[len-1] = '\0';
return( sofar );
}