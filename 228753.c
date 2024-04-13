static void appendnames(char *dest,char *dir,const char *dir_char,char *name,const char *ext ) {
    strcpy(dest,dir);
    dest += strlen(dest);
    strcpy(dest,dir_char);
    dest += strlen(dest);
    /* Some file systems are case-insensitive, so we can't just */
    /* copy the glyph name blindly (else "A" and "a" would map to the same file */
    for (;;) {
	if ( strncmp(name,"uni",3)==0 && ishexdigit(name[3]) && ishexdigit(name[4]) &&
		ishexdigit(name[5]) && ishexdigit(name[6])) {
	    /* but in a name like uni00AD case is irrelevant. Even under unix its */
	    /*  the same as uni00ad -- and it looks ugly */
	    strncpy(dest,name,7);
	    dest += 7; name += 7;
	    while ( ishexdigit(name[0]) && ishexdigit(name[1]) &&
		    ishexdigit(name[2]) && ishexdigit(name[3]) ) {
		strncpy(dest,name,4);
		dest += 4; name += 4;
	    }
	} else if ( name[0]=='u' && ishexdigit(name[1]) && ishexdigit(name[2]) &&
		ishexdigit(name[3]) && ishexdigit(name[4]) &&
		ishexdigit(name[5]) ) {
	    strncpy(dest,name,5);
	    dest += 5; name += 5;
	} else
    break;
	if ( *name!='_' )
    break;
	*dest++ = '_';
	++name;
    }
    while ( *name ) {
	if ( isupper(*name)) {
	    *dest++ = '_';
	    *dest++ = *name;
	} else
	    *dest++ = *name;
	++name;
    }
    strcpy(dest,ext);
}