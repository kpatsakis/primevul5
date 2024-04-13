pixReadMemTiff(const l_uint8  *cdata,
               size_t          size,
               l_int32         n)
{
l_uint8  *data;
l_int32   i;
PIX      *pix;
TIFF     *tif;

    PROCNAME("pixReadMemTiff");

    if (!cdata)
        return (PIX *)ERROR_PTR("cdata not defined", procName, NULL);

    data = (l_uint8 *)cdata;  /* we're really not going to change this */
    if ((tif = fopenTiffMemstream("tifferror", "r", &data, &size)) == NULL)
        return (PIX *)ERROR_PTR("tiff stream not opened", procName, NULL);

    pix = NULL;
    for (i = 0; ; i++) {
        if (i == n) {
            if ((pix = pixReadFromTiffStream(tif)) == NULL) {
                TIFFClose(tif);
                return NULL;
            }
            pixSetInputFormat(pix, IFF_TIFF);
            break;
        }
        if (TIFFReadDirectory(tif) == 0)
            break;
        if (i == ManyPagesInTiffFile + 1) {
            L_WARNING("big file: more than %d pages\n", procName,
                      ManyPagesInTiffFile);
        }
    }

    TIFFClose(tif);
    return pix;
}