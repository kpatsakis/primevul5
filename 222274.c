openTiff(const char  *filename,
         const char  *modestring)
{
char  *fname;
TIFF  *tif;

    PROCNAME("openTiff");

    if (!filename)
        return (TIFF *)ERROR_PTR("filename not defined", procName, NULL);
    if (!modestring)
        return (TIFF *)ERROR_PTR("modestring not defined", procName, NULL);

    TIFFSetWarningHandler(NULL);  /* disable warnings */
    TIFFSetErrorHandler(NULL);  /* disable error messages */

    fname = genPathname(filename, NULL);
    tif = TIFFOpen(fname, modestring);
    LEPT_FREE(fname);
    return tif;
}