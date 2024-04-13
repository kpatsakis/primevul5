static struct gradient *SFDParseGradient(FILE *sfd,char *tok) {
    struct gradient *grad = chunkalloc(sizeof(struct gradient));
    int ch, i;

    getreal(sfd,&grad->start.x);
    while ( isspace(ch=nlgetc(sfd)));
    if ( ch!=';' ) ungetc(ch,sfd);
    getreal(sfd,&grad->start.y);

    getreal(sfd,&grad->stop.x);
    while ( isspace(ch=nlgetc(sfd)));
    if ( ch!=';' ) ungetc(ch,sfd);
    getreal(sfd,&grad->stop.y);

    getreal(sfd,&grad->radius);

    getname(sfd,tok);
    for ( i=0; spreads[i]!=NULL; ++i )
	if ( strmatch(spreads[i],tok)==0 )
    break;
    if ( spreads[i]==NULL ) i=0;
    grad->sm = i;

    getint(sfd,&grad->stop_cnt);
    grad->grad_stops = calloc(grad->stop_cnt,sizeof(struct grad_stops));
    for ( i=0; i<grad->stop_cnt; ++i ) {
	while ( isspace(ch=nlgetc(sfd)));
	if ( ch!='{' ) ungetc(ch,sfd);
	getreal( sfd, &grad->grad_stops[i].offset );
	gethex( sfd, &grad->grad_stops[i].col );
	getreal( sfd, &grad->grad_stops[i].opacity );
	while ( isspace(ch=nlgetc(sfd)));
	if ( ch!='}' ) ungetc(ch,sfd);
    }
return( grad );
}