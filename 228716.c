static void SFDGetPrivate(FILE *sfd,SplineFont *sf) {
    int i, cnt, len;
    char name[200];
    char *pt, *end;

    sf->private = calloc(1,sizeof(struct psdict));
    getint(sfd,&cnt);
    sf->private->next = sf->private->cnt = cnt;
    sf->private->values = calloc(cnt,sizeof(char *));
    sf->private->keys = calloc(cnt,sizeof(char *));
    for ( i=0; i<cnt; ++i ) {
	getname(sfd,name);
	sf->private->keys[i] = copy(name);
	getint(sfd,&len);
	nlgetc(sfd);	/* skip space */
	pt = sf->private->values[i] = malloc(len+1);
	for ( end = pt+len; pt<end; ++pt )
	    *pt = nlgetc(sfd);
	*pt='\0';
    }
}