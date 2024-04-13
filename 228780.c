char* FileToAllocatedString( FILE *f ) {
    char *ret, *buf;
    long fsize = 0;
    size_t bread = 0;

    /* get approximate file size, and allocate some memory */
    if ( fseek(f,0,SEEK_END)==0 && \
	 (fsize=ftell(f))!=-1   && \
	 fseek(f,0,SEEK_SET)==0 && \
	 (buf=calloc(fsize+30001,1))!=NULL ) {
	/* fread in file, size=non-exact, then resize memory smaller */
	bread=fread(buf,1,fsize+30000,f);
	if ( bread<=0 || bread >=(size_t)fsize+30000 || (ret=realloc(buf,bread+1))==NULL ) {
	    free( buf );
	} else {
	    ret[bread] = '\0';
	    return( ret );
	}
    }

    /* error occurred reading in file */
    fprintf(stderr,_("Failed to read a file. Bytes read:%ld file size:%ld\n"),(long)(bread),fsize );
    return( 0 );
}