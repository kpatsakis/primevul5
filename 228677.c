static void SFDDumpBitmapChar(FILE *sfd,BDFChar *bfc, int enc,int *newgids) {
    struct enc85 encrypt;
    int i;

    fprintf(sfd, "BDFChar: %d %d %d %d %d %d %d",
	    newgids!=NULL ? newgids[bfc->orig_pos] : bfc->orig_pos, enc,
	    bfc->width, bfc->xmin, bfc->xmax, bfc->ymin, bfc->ymax );
    if ( bfc->sc->parent->hasvmetrics )
	fprintf(sfd, " %d", bfc->vwidth);
    putc('\n',sfd);
    memset(&encrypt,'\0',sizeof(encrypt));
    encrypt.sfd = sfd;
    for ( i=0; i<=bfc->ymax-bfc->ymin; ++i ) {
	uint8 *pt = (uint8 *) (bfc->bitmap + i*bfc->bytes_per_line);
	uint8 *end = pt + bfc->bytes_per_line;
	while ( pt<end ) {
	    SFDEnc85(&encrypt,*pt);
	    ++pt;
	}
    }
    SFDEnc85EndEnc(&encrypt);
    fputc('\n',sfd);
}