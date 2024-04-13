tiffGetCount(FILE     *fp,
             l_int32  *pn)
{
l_int32  i;
TIFF    *tif;

    PROCNAME("tiffGetCount");

    if (!fp)
        return ERROR_INT("stream not defined", procName, 1);
    if (!pn)
        return ERROR_INT("&n not defined", procName, 1);
    *pn = 0;

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return ERROR_INT("tif not open for read", procName, 1);

    for (i = 1; ; i++) {
        if (TIFFReadDirectory(tif) == 0)
            break;
        if (i == ManyPagesInTiffFile + 1) {
            L_WARNING("big file: more than %d pages\n", procName,
                      ManyPagesInTiffFile);
        }
    }
    *pn = i;
    TIFFCleanup(tif);
    return 0;
}