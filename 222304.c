pixReadMemFromMultipageTiff(const l_uint8  *cdata,
                            size_t          size,
                            size_t         *poffset)
{
l_uint8  *data;
l_int32   retval;
size_t    offset;
PIX      *pix;
TIFF     *tif;

    PROCNAME("pixReadMemFromMultipageTiff");

    if (!cdata)
        return (PIX *)ERROR_PTR("cdata not defined", procName, NULL);
    if (!poffset)
        return (PIX *)ERROR_PTR("&offset not defined", procName, NULL);

    data = (l_uint8 *)cdata;  /* we're really not going to change this */
    if ((tif = fopenTiffMemstream("tifferror", "r", &data, &size)) == NULL)
        return (PIX *)ERROR_PTR("tiff stream not opened", procName, NULL);

        /* Set ptrs in the TIFF to the beginning of the image */
    offset = *poffset;
    retval = (offset == 0) ? TIFFSetDirectory(tif, 0)
                           : TIFFSetSubDirectory(tif, offset);
    if (retval == 0) {
        TIFFClose(tif);
        return NULL;
    }

    if ((pix = pixReadFromTiffStream(tif)) == NULL) {
        TIFFClose(tif);
        return NULL;
    }

        /* Advance to the next image and return the new offset */
    TIFFReadDirectory(tif);
    *poffset = TIFFCurrentDirOffset(tif);
    TIFFClose(tif);
    return pix;
}