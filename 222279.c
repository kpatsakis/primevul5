pixWriteTiffCustom(const char  *filename,
                   PIX         *pix,
                   l_int32      comptype,
                   const char  *modestr,
                   NUMA        *natags,
                   SARRAY      *savals,
                   SARRAY      *satypes,
                   NUMA        *nasizes)
{
l_int32  ret;
TIFF    *tif;

    PROCNAME("pixWriteTiffCustom");

    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!pix)
        return ERROR_INT("pix not defined", procName, 1);

    if ((tif = openTiff(filename, modestr)) == NULL)
        return ERROR_INT("tif not opened", procName, 1);
    ret = pixWriteToTiffStream(tif, pix, comptype, natags, savals,
                               satypes, nasizes);
    TIFFClose(tif);
    return ret;
}