static void SFDParseVertexKern(FILE *sfd, struct mathkernvertex *vertex) {
    int i,ch;

    getint(sfd,&vertex->cnt);
    vertex->mkd = calloc(vertex->cnt,sizeof(struct mathkerndata));
    for ( i=0; i<vertex->cnt; ++i ) {
	SFDParseMathValueRecord(sfd,&vertex->mkd[i].height,&vertex->mkd[i].height_adjusts);
	while ( (ch=nlgetc(sfd))==' ' );
	if ( ch!=EOF && ch!=',' )
	    ungetc(ch,sfd);
	SFDParseMathValueRecord(sfd,&vertex->mkd[i].kern,&vertex->mkd[i].kern_adjusts);
    }
}