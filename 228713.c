int SFDWriteBakExtended(char* locfilename,
			SplineFont *sf,EncMap *map,EncMap *normal,
			int s2d,
			int localRevisionsToRetain )
{
    int rc = 0;

    if( s2d )
    {
	rc = SFDWrite(locfilename,sf,map,normal,s2d);
	return rc;
    }


    int cacheRevisionsToRetain = prefRevisionsToRetain;

    sf->save_to_dir = s2d;

    if( localRevisionsToRetain < 0 )
    {
	// If there are no backups, then don't start creating any
	if( !SFDDoesAnyBackupExist(sf->filename))
	    prefRevisionsToRetain = 0;
    }
    else
    {
	prefRevisionsToRetain = localRevisionsToRetain;
    }

    rc = SFDWriteBak( locfilename, sf, map, normal );

    prefRevisionsToRetain = cacheRevisionsToRetain;

    return rc;
}