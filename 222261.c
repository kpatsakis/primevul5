fprintTiffInfo(FILE        *fpout,
               const char  *tiffile)
{
TIFF  *tif;

    PROCNAME("fprintTiffInfo");

    if (!tiffile)
        return ERROR_INT("tiffile not defined", procName, 1);
    if (!fpout)
        return ERROR_INT("stream out not defined", procName, 1);

    if ((tif = openTiff(tiffile, "rb")) == NULL)
        return ERROR_INT("tif not open for read", procName, 1);

    TIFFPrintDirectory(tif, fpout, 0);
    TIFFClose(tif);

    return 0;
}