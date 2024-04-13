readHeaderTiff(const char *filename,
               l_int32     n,
               l_int32    *pw,
               l_int32    *ph,
               l_int32    *pbps,
               l_int32    *pspp,
               l_int32    *pres,
               l_int32    *pcmap,
               l_int32    *pformat)
{
l_int32  ret;
FILE    *fp;

    PROCNAME("readHeaderTiff");

    if (pw) *pw = 0;
    if (ph) *ph = 0;
    if (pbps) *pbps = 0;
    if (pspp) *pspp = 0;
    if (pres) *pres = 0;
    if (pcmap) *pcmap = 0;
    if (pformat) *pformat = 0;
    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!pw && !ph && !pbps && !pspp && !pres && !pcmap && !pformat)
        return ERROR_INT("no results requested", procName, 1);

    if ((fp = fopenReadStream(filename)) == NULL)
        return ERROR_INT("image file not found", procName, 1);
    ret = freadHeaderTiff(fp, n, pw, ph, pbps, pspp, pres, pcmap, pformat);
    fclose(fp);
    return ret;
}