findTiffCompression(FILE     *fp,
                    l_int32  *pcomptype)
{
l_uint16  tiffcomp;
TIFF     *tif;

    PROCNAME("findTiffCompression");

    if (!pcomptype)
        return ERROR_INT("&comptype not defined", procName, 1);
    *pcomptype = IFF_UNKNOWN;  /* init */
    if (!fp)
        return ERROR_INT("stream not defined", procName, 1);

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return ERROR_INT("tif not opened", procName, 1);
    TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &tiffcomp);
    *pcomptype = getTiffCompressedFormat(tiffcomp);
    TIFFCleanup(tif);
    return 0;
}