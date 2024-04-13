static void SFDirClean(char *filename) {
    DIR *dir;
    struct dirent *ent;
    char *buffer, *pt;

    unlink(filename);		/* Just in case it's a normal file, it shouldn't be, but just in case... */
    dir = opendir(filename);
    if ( dir==NULL )
return;
    buffer = malloc(strlen(filename)+1+NAME_MAX+1);
    while ( (ent = readdir(dir))!=NULL ) {
	if ( strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0 )
    continue;
	pt = strrchr(ent->d_name,EXT_CHAR);
	if ( pt==NULL )
    continue;
	sprintf( buffer,"%s/%s", filename, ent->d_name );
	if ( strcmp(pt,".props")==0 ||
		strcmp(pt,GLYPH_EXT)==0 ||
		strcmp(pt,BITMAP_EXT)==0 )
	    unlink( buffer );
	else if ( strcmp(pt,STRIKE_EXT)==0 ||
		strcmp(pt,SUBFONT_EXT)==0 ||
		strcmp(pt,INSTANCE_EXT)==0 )
	    SFDirClean(buffer);
	/* If there are filenames we don't recognize, leave them. They might contain version control info */
    }
    free(buffer);
    closedir(dir);
}