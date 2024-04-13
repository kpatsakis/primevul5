static void SFDDumpImagePNG(FILE *sfd,ImageList *img) {
    struct enc85 enc = {0};
    char* pngbuf;
    size_t pnglen, i;

    if (!GImageWritePngBuf(img->image, &pngbuf, &pnglen, 1, false)) {
        IError("Failed to serialise PNG image");
        return;
    }

    fprintf(sfd, "Image2: image/png %d %g %g %g %g\n",
        (int)pnglen, (double) img->xoff, (double) img->yoff, (double) img->xscale, (double) img->yscale );

    enc.sfd = sfd;
    for (i = 0; i<pnglen; ++i) {
        SFDEnc85(&enc, pngbuf[i]);
    }
    free(pngbuf);

    SFDEnc85EndEnc(&enc);
    fprintf(sfd,"\nEndImage2\n" );
}