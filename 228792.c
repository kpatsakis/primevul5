static void SFFinalDirClean(char *filename) {
    DIR *dir;
    struct dirent *ent;
    char *buffer, *markerfile, *pt;

    /* we did not unlink sub-directories in case they contained version control */
    /*  files. We did remove all our files from them, however.  If the user */
    /*  removed a bitmap strike or a cid-subfont those sub-dirs will now be */
    /*  empty. If the user didn't remove them then they will contain our marker */
    /*  files. So if we find a subdir with no marker files in it, remove it */
    dir = opendir(filename);
    if ( dir==NULL )
return;
    buffer = malloc(strlen(filename)+1+NAME_MAX+1);
    markerfile = malloc(strlen(filename)+2+2*NAME_MAX+1);
    while ( (ent = readdir(dir))!=NULL ) {
	if ( strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0 )
    continue;
	pt = strrchr(ent->d_name,EXT_CHAR);
	if ( pt==NULL )
    continue;
	sprintf( buffer,"%s/%s", filename, ent->d_name );
	if ( strcmp(pt,".strike")==0 ||
		strcmp(pt,SUBFONT_EXT)==0 ||
		strcmp(pt,INSTANCE_EXT)==0 ) {
	    if ( strcmp(pt,".strike")==0 )
		sprintf( markerfile,"%s/" STRIKE_PROPS, buffer );
	    else
		sprintf( markerfile,"%s/" FONT_PROPS, buffer );
	    if ( !GFileExists(markerfile)) {
		GFileRemove(buffer, false);
	    }
	}
    }
    free(buffer);
    free(markerfile);
    closedir(dir);
}