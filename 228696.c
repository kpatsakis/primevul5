static struct macname *SFDParseMacNames(FILE *sfd, char *tok) {
    struct macname *head=NULL, *last=NULL, *cur;
    int enc, lang, len;
    char *pt;
    int ch;

    while ( strcmp(tok,"MacName:")==0 ) {
	cur = chunkalloc(sizeof(struct macname));
	if ( last==NULL )
	    head = cur;
	else
	    last->next = cur;
	last = cur;

	getint(sfd,&enc);
	getint(sfd,&lang);
	getint(sfd,&len);
	cur->enc = enc;
	cur->lang = lang;
	cur->name = pt = malloc(len+1);

	while ( (ch=nlgetc(sfd))==' ');
	if ( ch=='"' )
	    ch = nlgetc(sfd);
	while ( ch!='"' && ch!=EOF && pt<cur->name+len ) {
	    if ( ch=='\\' ) {
		*pt  = (nlgetc(sfd)-'0')<<6;
		*pt |= (nlgetc(sfd)-'0')<<3;
		*pt |= (nlgetc(sfd)-'0');
	    } else
		*pt++ = ch;
	    ch = nlgetc(sfd);
	}
	*pt = '\0';
	getname(sfd,tok);
    }
return( head );
}