readHeaderMemTiff(const l_uint8  *cdata,
                  size_t          size,
                  l_int32         n,
                  l_int32        *pw,
                  l_int32        *ph,
                  l_int32        *pbps,
                  l_int32        *pspp,
                  l_int32        *pres,
                  l_int32        *pcmap,
                  l_int32        *pformat)
{
l_uint8  *data;
l_int32   i, ret;
TIFF     *tif;

    PROCNAME("readHeaderMemTiff");

    if (pw) *pw = 0;
    if (ph) *ph = 0;
    if (pbps) *pbps = 0;
    if (pspp) *pspp = 0;
    if (pres) *pres = 0;
    if (pcmap) *pcmap = 0;
    if (pformat) *pformat = 0;
    if (!pw && !ph && !pbps && !pspp && !pres && !pcmap && !pformat)
        return ERROR_INT("no results requested", procName, 1);
    if (!cdata)
        return ERROR_INT("cdata not defined", procName, 1);

        /* Open a tiff stream to memory */
    data = (l_uint8 *)cdata;  /* we're really not going to change this */
    if ((tif = fopenTiffMemstream("tifferror", "r", &data, &size)) == NULL)
        return ERROR_INT("tiff stream not opened", procName, 1);

    for (i = 0; i < n; i++) {
        if (TIFFReadDirectory(tif) == 0) {
            TIFFClose(tif);
            return ERROR_INT("image n not found in file", procName, 1);
        }
    }

    ret = tiffReadHeaderTiff(tif, pw, ph, pbps, pspp, pres, pcmap, pformat);
    TIFFClose(tif);
    return ret;
}