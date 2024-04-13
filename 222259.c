pixaWriteMultipageTiff(const char  *fname,
                       PIXA        *pixa)
{
const char  *modestr;
l_int32      i, n;
PIX         *pix1;

    PROCNAME("pixaWriteMultipageTiff");

    if (!fname)
        return ERROR_INT("fname not defined", procName, 1);
    if (!pixa)
        return ERROR_INT("pixa not defined", procName, 1);

    n = pixaGetCount(pixa);
    for (i = 0; i < n; i++) {
        modestr = (i == 0) ? "w" : "a";
        pix1 = pixaGetPix(pixa, i, L_CLONE);
        if (pixGetDepth(pix1) == 1)
            pixWriteTiff(fname, pix1, IFF_TIFF_G4, modestr);
        else
            pixWriteTiff(fname, pix1, IFF_TIFF_ZIP, modestr);
        pixDestroy(&pix1);
    }

    return 0;
}