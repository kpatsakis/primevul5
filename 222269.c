freadHeaderTiff(FILE     *fp,
                l_int32   n,
                l_int32  *pw,
                l_int32  *ph,
                l_int32  *pbps,
                l_int32  *pspp,
                l_int32  *pres,
                l_int32  *pcmap,
                l_int32  *pformat)
{
l_int32  i, ret, format;
TIFF    *tif;

    PROCNAME("freadHeaderTiff");

    if (pw) *pw = 0;
    if (ph) *ph = 0;
    if (pbps) *pbps = 0;
    if (pspp) *pspp = 0;
    if (pres) *pres = 0;
    if (pcmap) *pcmap = 0;
    if (pformat) *pformat = 0;
    if (!fp)
        return ERROR_INT("stream not defined", procName, 1);
    if (n < 0)
        return ERROR_INT("image index must be >= 0", procName, 1);
    if (!pw && !ph && !pbps && !pspp && !pres && !pcmap && !pformat)
        return ERROR_INT("no results requested", procName, 1);

    findFileFormatStream(fp, &format);
    if (!L_FORMAT_IS_TIFF(format))
        return ERROR_INT("file not tiff format", procName, 1);

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return ERROR_INT("tif not open for read", procName, 1);

    for (i = 0; i < n; i++) {
        if (TIFFReadDirectory(tif) == 0)
            return ERROR_INT("image n not found in file", procName, 1);
    }

    ret = tiffReadHeaderTiff(tif, pw, ph, pbps, pspp, pres, pcmap, pformat);
    TIFFCleanup(tif);
    return ret;
}