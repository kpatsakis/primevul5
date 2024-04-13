static int SFD_MIDump(SplineFont *sf,EncMap *map,char *dirname,	int mm_pos) {
    char *instance = malloc(strlen(dirname)+1+10+20);
    char *fontprops;
    FILE *ssfd;
    int err = false;

    /* I'd like to use the font name, but the order of the instances is */
    /*  crucial and I must enforce an ordering on them */
    sprintf( instance,"%s/mm%d" INSTANCE_EXT, dirname, mm_pos );
    GFileMkDir(instance, 0755);
    fontprops = malloc(strlen(instance)+strlen("/" FONT_PROPS)+1);
    strcpy(fontprops,instance); strcat(fontprops,"/" FONT_PROPS);
    ssfd = fopen( fontprops,"w");
    if ( ssfd!=NULL ) {
	err |= SFD_Dump(ssfd,sf,map,NULL,true,instance);
	if ( ferror(ssfd) ) err = true;
	if ( fclose(ssfd)) err = true;
    } else
	err = true;
    free(fontprops);
    free(instance);
return( err );
}