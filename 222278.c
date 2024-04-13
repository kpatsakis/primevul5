fopenTiffMemstream(const char  *filename,
                   const char  *operation,
                   l_uint8    **pdata,
                   size_t      *pdatasize)
{
L_MEMSTREAM  *mstream;
TIFF         *tif;

    PROCNAME("fopenTiffMemstream");

    if (!filename)
        return (TIFF *)ERROR_PTR("filename not defined", procName, NULL);
    if (!operation)
        return (TIFF *)ERROR_PTR("operation not defined", procName, NULL);
    if (!pdata)
        return (TIFF *)ERROR_PTR("&data not defined", procName, NULL);
    if (!pdatasize)
        return (TIFF *)ERROR_PTR("&datasize not defined", procName, NULL);
    if (strcmp(operation, "r") && strcmp(operation, "w"))
        return (TIFF *)ERROR_PTR("op not 'r' or 'w'", procName, NULL);

    if (!strcmp(operation, "r"))
        mstream = memstreamCreateForRead(*pdata, *pdatasize);
    else
        mstream = memstreamCreateForWrite(pdata, pdatasize);

    TIFFSetWarningHandler(NULL);  /* disable warnings */
    TIFFSetErrorHandler(NULL);  /* disable error messages */

    tif = TIFFClientOpen(filename, operation, (thandle_t)mstream,
                         tiffReadCallback, tiffWriteCallback,
                         tiffSeekCallback, tiffCloseCallback,
                         tiffSizeCallback, tiffMapCallback,
                         tiffUnmapCallback);
    if (!tif)
        LEPT_FREE(mstream);
    return tif;
}