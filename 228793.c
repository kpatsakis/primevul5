static struct ttflangname *SFDGetLangName(FILE *sfd,struct ttflangname *old) {
    struct ttflangname *cur = chunkalloc(sizeof(struct ttflangname)), *prev;
    int i;

    getint(sfd,&cur->lang);
    for ( i=0; i<ttf_namemax; ++i )
	cur->names[i] = SFDReadUTF7Str(sfd);
    if ( old==NULL )
return( cur );
    for ( prev = old; prev->next !=NULL; prev = prev->next );
    prev->next = cur;
return( old );
}