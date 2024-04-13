pixWriteToTiffStream(TIFF    *tif,
                     PIX     *pix,
                     l_int32  comptype,
                     NUMA    *natags,
                     SARRAY  *savals,
                     SARRAY  *satypes,
                     NUMA    *nasizes)
{
l_uint8   *linebuf, *data;
l_uint16   redmap[256], greenmap[256], bluemap[256];
l_int32    w, h, d, spp, i, j, k, wpl, bpl, tiffbpl, ncolors, cmapsize;
l_int32   *rmap, *gmap, *bmap;
l_int32    xres, yres;
l_uint32  *line, *ppixel;
PIX       *pixt;
PIXCMAP   *cmap;
char      *text;

    PROCNAME("pixWriteToTiffStream");

    if (!tif)
        return ERROR_INT("tif stream not defined", procName, 1);
    if (!pix)
        return ERROR_INT( "pix not defined", procName, 1 );

    pixSetPadBits(pix, 0);
    pixGetDimensions(pix, &w, &h, &d);
    spp = pixGetSpp(pix);
    xres = pixGetXRes(pix);
    yres = pixGetYRes(pix);
    if (xres == 0) xres = DefaultResolution;
    if (yres == 0) yres = DefaultResolution;

        /* ------------------ Write out the header -------------  */
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, (l_uint32)RESUNIT_INCH);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, (l_float64)xres);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, (l_float64)yres);

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (l_uint32)w);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (l_uint32)h);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    if ((text = pixGetText(pix)) != NULL)
        TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, text);

    if (d == 1 && !pixGetColormap(pix)) {
            /* If d == 1, preserve the colormap.  Note that when
             * d == 1 pix with colormaps are read, the colormaps
             * are removed.  The only pix in leptonica that have
             * colormaps are made programmatically. */
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
    } else if ((d == 32 && spp == 3) || d == 24) {
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (l_uint16)3);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,
                       (l_uint16)8, (l_uint16)8, (l_uint16)8);
    } else if (d == 32 && spp == 4) {
        l_uint16  val[1];
        val[0] = EXTRASAMPLE_ASSOCALPHA;
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, (l_uint16)1, &val);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (l_uint16)4);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,
                     (l_uint16)8, (l_uint16)8, (l_uint16)8, (l_uint16)8);
    } else if (d == 16) {  /* we only support spp = 1, bps = 16 */
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    } else if ((cmap = pixGetColormap(pix)) == NULL) {
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    } else {  /* Save colormap in the tiff; not more than 256 colors */
        if (d > 8) {
            L_ERROR("d = %d > 8 with colormap!; reducing to 8\n", procName, d);
            d = 8;
        }
        pixcmapToArrays(cmap, &rmap, &gmap, &bmap, NULL);
        ncolors = pixcmapGetCount(cmap);
        ncolors = L_MIN(256, ncolors);  /* max 256 */
        cmapsize = 1 << d;
        cmapsize = L_MIN(256, cmapsize);  /* power of 2; max 256 */
        if (ncolors > cmapsize) {
            L_WARNING("too many colors in cmap for tiff; truncating\n",
                      procName);
            ncolors = cmapsize;
        }
        for (i = 0; i < ncolors; i++) {
            redmap[i] = (rmap[i] << 8) | rmap[i];
            greenmap[i] = (gmap[i] << 8) | gmap[i];
            bluemap[i] = (bmap[i] << 8) | bmap[i];
        }
        for (i = ncolors; i < cmapsize; i++)  /* init, even though not used */
            redmap[i] = greenmap[i] = bluemap[i] = 0;
        LEPT_FREE(rmap);
        LEPT_FREE(gmap);
        LEPT_FREE(bmap);

        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (l_uint16)1);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (l_uint16)d);
        TIFFSetField(tif, TIFFTAG_COLORMAP, redmap, greenmap, bluemap);
    }

    if (d <= 16) {
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (l_uint16)d);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (l_uint16)1);
    }

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if (comptype == IFF_TIFF) {  /* no compression */
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    } else if (comptype == IFF_TIFF_G4) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
    } else if (comptype == IFF_TIFF_G3) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX3);
    } else if (comptype == IFF_TIFF_RLE) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_CCITTRLE);
    } else if (comptype == IFF_TIFF_PACKBITS) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
    } else if (comptype == IFF_TIFF_LZW) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    } else if (comptype == IFF_TIFF_ZIP) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
    } else if (comptype == IFF_TIFF_JPEG) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
    } else {
        L_WARNING("unknown tiff compression; using none\n", procName);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    }

        /* This is a no-op if arrays are NULL */
    writeCustomTiffTags(tif, natags, savals, satypes, nasizes);

        /* ------------- Write out the image data -------------  */
    tiffbpl = TIFFScanlineSize(tif);
    wpl = pixGetWpl(pix);
    bpl = 4 * wpl;
    if (tiffbpl > bpl)
        lept_stderr("Big trouble: tiffbpl = %d, bpl = %d\n", tiffbpl, bpl);
    if ((linebuf = (l_uint8 *)LEPT_CALLOC(1, bpl)) == NULL)
        return ERROR_INT("calloc fail for linebuf", procName, 1);

        /* Use single strip for image */
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, h);

    if (d != 24 && d != 32) {
        if (d == 16)
            pixt = pixEndianTwoByteSwapNew(pix);
        else
            pixt = pixEndianByteSwapNew(pix);
        data = (l_uint8 *)pixGetData(pixt);
        for (i = 0; i < h; i++, data += bpl) {
            memcpy(linebuf, data, tiffbpl);
            if (TIFFWriteScanline(tif, linebuf, i, 0) < 0)
                break;
        }
        pixDestroy(&pixt);
    } else if (d == 24) {  /* See note 4 above: special case of 24 bpp rgb */
        for (i = 0; i < h; i++) {
            line = pixGetData(pix) + i * wpl;
            if (TIFFWriteScanline(tif, (l_uint8 *)line, i, 0) < 0)
                break;
        }
    } else {  /* 32 bpp rgb or rgba */
        for (i = 0; i < h; i++) {
            line = pixGetData(pix) + i * wpl;
            for (j = 0, k = 0, ppixel = line; j < w; j++) {
                linebuf[k++] = GET_DATA_BYTE(ppixel, COLOR_RED);
                linebuf[k++] = GET_DATA_BYTE(ppixel, COLOR_GREEN);
                linebuf[k++] = GET_DATA_BYTE(ppixel, COLOR_BLUE);
                if (spp == 4)
                    linebuf[k++] = GET_DATA_BYTE(ppixel, L_ALPHA_CHANNEL);
                ppixel++;
            }
            if (TIFFWriteScanline(tif, linebuf, i, 0) < 0)
                break;
        }
    }

/*    TIFFWriteDirectory(tif); */
    LEPT_FREE(linebuf);

    return 0;
}