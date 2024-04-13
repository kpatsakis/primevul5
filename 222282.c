pixReadFromMultipageTiff(const char  *fname,
                         size_t      *poffset)
{
l_int32  retval;
size_t   offset;
PIX     *pix;
TIFF    *tif;

    PROCNAME("pixReadFromMultipageTiff");

    if (!fname)
        return (PIX *)ERROR_PTR("fname not defined", procName, NULL);
    if (!poffset)
        return (PIX *)ERROR_PTR("&offset not defined", procName, NULL);

    if ((tif = openTiff(fname, "r")) == NULL) {
        L_ERROR("tif open failed for %s\n", procName, fname);
        return NULL;
    }

        /* Set ptrs in the TIFF to the beginning of the image */
    offset = *poffset;
    retval = (offset == 0) ? TIFFSetDirectory(tif, 0)
                            : TIFFSetSubDirectory(tif, offset);
    if (retval == 0) {
        TIFFCleanup(tif);
        return NULL;
    }

    if ((pix = pixReadFromTiffStream(tif)) == NULL) {
        TIFFCleanup(tif);
        return NULL;
    }

        /* Advance to the next image and return the new offset */
    TIFFReadDirectory(tif);
    *poffset = TIFFCurrentDirOffset(tif);
    TIFFClose(tif);
    return pix;
}