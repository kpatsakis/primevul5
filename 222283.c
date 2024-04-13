getTiffStreamResolution(TIFF     *tif,
                        l_int32  *pxres,
                        l_int32  *pyres)
{
l_uint16   resunit;
l_int32    foundxres, foundyres;
l_float32  fxres, fyres;

    PROCNAME("getTiffStreamResolution");

    if (!tif)
        return ERROR_INT("tif not opened", procName, 1);
    if (!pxres || !pyres)
        return ERROR_INT("&xres and &yres not both defined", procName, 1);
    *pxres = *pyres = 0;

    TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &resunit);
    foundxres = TIFFGetField(tif, TIFFTAG_XRESOLUTION, &fxres);
    foundyres = TIFFGetField(tif, TIFFTAG_YRESOLUTION, &fyres);
    if (!foundxres && !foundyres) return 1;
    if (isnan(fxres) || isnan(fyres)) return 1;
    if (!foundxres && foundyres)
        fxres = fyres;
    else if (foundxres && !foundyres)
        fyres = fxres;

        /* Avoid overflow into int32; set max fxres and fyres to 5 x 10^8 */
    if (fxres < 0 || fxres > (1L << 29) || fyres < 0 || fyres > (1L << 29))
        return ERROR_INT("fxres and/or fyres values are invalid", procName, 1);

    if (resunit == RESUNIT_CENTIMETER) {  /* convert to ppi */
        *pxres = (l_int32)(2.54 * fxres + 0.5);
        *pyres = (l_int32)(2.54 * fyres + 0.5);
    } else {
        *pxres = (l_int32)fxres;
        *pyres = (l_int32)fyres;
    }

    return 0;
}