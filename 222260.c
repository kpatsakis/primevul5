fopenTiff(FILE        *fp,
          const char  *modestring)
{
    PROCNAME("fopenTiff");

    if (!fp)
        return (TIFF *)ERROR_PTR("stream not opened", procName, NULL);
    if (!modestring)
        return (TIFF *)ERROR_PTR("modestring not defined", procName, NULL);

    TIFFSetWarningHandler(NULL);  /* disable warnings */
    TIFFSetErrorHandler(NULL);  /* disable error messages */

    fseek(fp, 0, SEEK_SET);
    return TIFFClientOpen("TIFFstream", modestring, (thandle_t)fp,
                          lept_read_proc, lept_write_proc, lept_seek_proc,
                          lept_close_proc, lept_size_proc, NULL, NULL);
}