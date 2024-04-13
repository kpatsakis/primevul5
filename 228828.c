static struct pattern *SFDParsePattern(FILE *sfd,char *tok) {
    struct pattern *pat = chunkalloc(sizeof(struct pattern));
    int ch;

    getname(sfd,tok);
    pat->pattern = copy(tok);

    getreal(sfd,&pat->width);
    while ( isspace(ch=nlgetc(sfd)));
    if ( ch!=';' ) ungetc(ch,sfd);
    getreal(sfd,&pat->height);

    while ( isspace(ch=nlgetc(sfd)));
    if ( ch!='[' ) ungetc(ch,sfd);
    getreal(sfd,&pat->transform[0]);
    getreal(sfd,&pat->transform[1]);
    getreal(sfd,&pat->transform[2]);
    getreal(sfd,&pat->transform[3]);
    getreal(sfd,&pat->transform[4]);
    getreal(sfd,&pat->transform[5]);
    while ( isspace(ch=nlgetc(sfd)));
    if ( ch!=']' ) ungetc(ch,sfd);
return( pat );
}