pixReadStreamTiff(FILE    *fp,
                  l_int32  n)
{
PIX   *pix;
TIFF  *tif;

    PROCNAME("pixReadStreamTiff");

    if (!fp)
        return (PIX *)ERROR_PTR("stream not defined", procName, NULL);

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return (PIX *)ERROR_PTR("tif not opened", procName, NULL);

    if (TIFFSetDirectory(tif, n) == 0) {
        TIFFCleanup(tif);
        return NULL;
    }
    if ((pix = pixReadFromTiffStream(tif)) == NULL) {
        TIFFCleanup(tif);
        return NULL;
    }
    TIFFCleanup(tif);
    return pix;
}