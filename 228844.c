static ImageList *SFDGetImagePNG(FILE *sfd) {
    int pnglen;
    ImageList *img;
    struct enc85 dec = {0};
    int i, ch;

    img = calloc(1,sizeof(ImageList));
    dec.pos = -1;
    dec.sfd = sfd;

    getint(sfd,&pnglen);
    getreal(sfd,&img->xoff);
    getreal(sfd,&img->yoff);
    getreal(sfd,&img->xscale);
    getreal(sfd,&img->yscale);

    while ( (ch=nlgetc(sfd))==' ' || ch=='\t' )
        /* skip */;

    char* pngbuf = malloc(pnglen * sizeof(char));
    if (pngbuf == NULL) {
        IError("Failed to allocate buffer to read PNG in SFD file");
        return NULL;
    }

    for (i = 0; i<pnglen; ++i) {
        pngbuf[i] = Dec85(&dec);
    }

    img->image = GImageReadPngBuf(pngbuf, pnglen);
    free(pngbuf);

    if (img->image == NULL) {
        IError("Failed to read PNG in SFD file, skipping it.");
        free(img);
        return NULL;
    }

    img->bb.minx = img->xoff; img->bb.maxy = img->yoff;
    img->bb.maxx = img->xoff + GImageGetWidth(img->image)*img->xscale;
    img->bb.miny = img->yoff - GImageGetHeight(img->image)*img->yscale;
    return img;
}