int SFDWriteBak(char *filename,SplineFont *sf,EncMap *map,EncMap *normal) {
    char *buf=0, *buf2=NULL;
    int ret;

    if ( sf->save_to_dir )
    {
	ret = SFDWrite(filename,sf,map,normal,true);
	return(ret);
    }

    if ( sf->cidmaster!=NULL )
	sf=sf->cidmaster;
    buf = malloc(strlen(filename)+10);
    if ( sf->compression!=0 )
    {
	buf2 = malloc(strlen(filename)+10);
	strcpy(buf2,filename);
	strcat(buf2,compressors[sf->compression-1].ext);
	strcpy(buf,buf2);
	strcat(buf,"~");
	if ( rename(buf2,buf)==0 )
	    sf->backedup = bs_backedup;
    }
    else
    {
	sf->backedup = bs_dontknow;

	if( prefRevisionsToRetain )
	{
	    char path[PATH_MAX];
	    char pathnew[PATH_MAX];
	    int idx = 0;

	    snprintf( path,    PATH_MAX, "%s", filename );
	    snprintf( pathnew, PATH_MAX, "%s-%02d", filename, idx );
	    (void)rename( path, pathnew );

	    for( idx=prefRevisionsToRetain; idx > 0; idx-- )
	    {
		snprintf( path, PATH_MAX, "%s-%02d", filename, idx-1 );
		snprintf( pathnew, PATH_MAX, "%s-%02d", filename, idx );

		int rc = rename( path, pathnew );
		if( !idx && !rc )
		    sf->backedup = bs_backedup;
	    }
	    idx = prefRevisionsToRetain+1;
	    snprintf( path, PATH_MAX, "%s-%02d", filename, idx );
	    unlink(path);
	}

    }
    free(buf);

    ret = SFDWrite(filename,sf,map,normal,false);
    if ( ret && sf->compression!=0 ) {
	unlink(buf2);
	buf = malloc(strlen(filename)+40);
	sprintf( buf, "%s %s", compressors[sf->compression-1].recomp, filename );
	if ( system( buf )!=0 )
	    sf->compression = 0;
	free(buf);
    }
    free(buf2);
    return( ret );
}