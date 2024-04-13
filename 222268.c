extractG4DataFromFile(const char  *filein,
                      l_uint8    **pdata,
                      size_t      *pnbytes,
                      l_int32     *pw,
                      l_int32     *ph,
                      l_int32     *pminisblack)
{
l_uint8  *inarray, *data;
l_uint16  minisblack, comptype;  /* accessors require l_uint16 */
l_int32   istiff;
l_uint32  w, h, rowsperstrip;  /* accessors require l_uint32 */
l_uint32  diroff;
size_t    fbytes, nbytes;
FILE     *fpin;
TIFF     *tif;

    PROCNAME("extractG4DataFromFile");

    if (!pdata)
        return ERROR_INT("&data not defined", procName, 1);
    if (!pnbytes)
        return ERROR_INT("&nbytes not defined", procName, 1);
    if (!pw && !ph && !pminisblack)
        return ERROR_INT("no output data requested", procName, 1);
    *pdata = NULL;
    *pnbytes = 0;

    if ((fpin = fopenReadStream(filein)) == NULL)
        return ERROR_INT("stream not opened to file", procName, 1);
    istiff = fileFormatIsTiff(fpin);
    fclose(fpin);
    if (!istiff)
        return ERROR_INT("filein not tiff", procName, 1);

    if ((inarray = l_binaryRead(filein, &fbytes)) == NULL)
        return ERROR_INT("inarray not made", procName, 1);

        /* Get metadata about the image */
    if ((tif = openTiff(filein, "rb")) == NULL) {
        LEPT_FREE(inarray);
        return ERROR_INT("tif not open for read", procName, 1);
    }
    TIFFGetField(tif, TIFFTAG_COMPRESSION, &comptype);
    if (comptype != COMPRESSION_CCITTFAX4) {
        LEPT_FREE(inarray);
        TIFFClose(tif);
        return ERROR_INT("filein is not g4 compressed", procName, 1);
    }

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    if (h != rowsperstrip)
        L_WARNING("more than 1 strip\n", procName);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &minisblack);  /* for 1 bpp */
/*    TIFFPrintDirectory(tif, stderr, 0); */
    TIFFClose(tif);
    if (pw) *pw = (l_int32)w;
    if (ph) *ph = (l_int32)h;
    if (pminisblack) *pminisblack = (l_int32)minisblack;

        /* The header has 8 bytes: the first 2 are the magic number,
         * the next 2 are the version, and the last 4 are the
         * offset to the first directory.  That's what we want here.
         * We have to test the byte order before decoding 4 bytes! */
    if (inarray[0] == 0x4d) {  /* big-endian */
        diroff = (inarray[4] << 24) | (inarray[5] << 16) |
                 (inarray[6] << 8) | inarray[7];
    } else  {   /* inarray[0] == 0x49 :  little-endian */
        diroff = (inarray[7] << 24) | (inarray[6] << 16) |
                 (inarray[5] << 8) | inarray[4];
    }
/*    lept_stderr(" diroff = %d, %x\n", diroff, diroff); */

        /* Extract the ccittg4 encoded data from the tiff file.
         * We skip the 8 byte header and take nbytes of data,
         * up to the beginning of the directory (at diroff)  */
    nbytes = diroff - 8;
    *pnbytes = nbytes;
    if ((data = (l_uint8 *)LEPT_CALLOC(nbytes, sizeof(l_uint8))) == NULL) {
        LEPT_FREE(inarray);
        return ERROR_INT("data not allocated", procName, 1);
    }
    *pdata = data;
    memcpy(data, inarray + 8, nbytes);
    LEPT_FREE(inarray);

    return 0;
}