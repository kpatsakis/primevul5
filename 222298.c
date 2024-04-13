getTiffResolution(FILE     *fp,
                  l_int32  *pxres,
                  l_int32  *pyres)
{
TIFF  *tif;

    PROCNAME("getTiffResolution");

    if (!pxres || !pyres)
        return ERROR_INT("&xres and &yres not both defined", procName, 1);
    *pxres = *pyres = 0;
    if (!fp)
        return ERROR_INT("stream not opened", procName, 1);

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return ERROR_INT("tif not open for read", procName, 1);
    getTiffStreamResolution(tif, pxres, pyres);
    TIFFCleanup(tif);
    return 0;
}