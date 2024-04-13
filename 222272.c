tiffReadHeaderTiff(TIFF     *tif,
                   l_int32  *pw,
                   l_int32  *ph,
                   l_int32  *pbps,
                   l_int32  *pspp,
                   l_int32  *pres,
                   l_int32  *pcmap,
                   l_int32  *pformat)
{
l_uint16   tiffcomp;
l_uint16   bps, spp;
l_uint16  *rmap, *gmap, *bmap;
l_int32    xres, yres;
l_uint32   w, h;

    PROCNAME("tiffReadHeaderTiff");

    if (!tif)
        return ERROR_INT("tif not opened", procName, 1);

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    if (w < 1 || h < 1)
        return ERROR_INT("tif w and h not both > 0", procName, 1);
    if (bps != 1 && bps != 2 && bps != 4 && bps != 8 && bps != 16)
        return ERROR_INT("bps not in set {1,2,4,8,16}", procName, 1);
    if (spp != 1 && spp != 2 && spp != 3 && spp != 4)
        return ERROR_INT("spp not in set {1,2,3,4}", procName, 1);
    if (pw) *pw = w;
    if (ph) *ph = h;
    if (pbps) *pbps = bps;
    if (pspp) *pspp = spp;
    if (pres) {
        *pres = 300;  /* default ppi */
        if (getTiffStreamResolution(tif, &xres, &yres) == 0)
            *pres = (l_int32)xres;
    }
    if (pcmap) {
        *pcmap = 0;
        if (TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap))
            *pcmap = 1;
    }
    if (pformat) {
        TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &tiffcomp);
        *pformat = getTiffCompressedFormat(tiffcomp);
    }
    return 0;
}