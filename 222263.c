pixWriteStreamTiffWA(FILE        *fp,
                     PIX         *pix,
                     l_int32      comptype,
                     const char  *modestr)
{
TIFF  *tif;

    PROCNAME("pixWriteStreamTiffWA");

    if (!fp)
        return ERROR_INT("stream not defined", procName, 1 );
    if (!pix)
        return ERROR_INT("pix not defined", procName, 1 );
    if (strcmp(modestr, "w") && strcmp(modestr, "a"))
        return ERROR_INT("modestr not 'w' or 'a'", procName, 1 );

    if (pixGetDepth(pix) != 1 && comptype != IFF_TIFF &&
        comptype != IFF_TIFF_LZW && comptype != IFF_TIFF_ZIP &&
        comptype != IFF_TIFF_JPEG) {
        L_WARNING("invalid compression type for bpp > 1\n", procName);
        comptype = IFF_TIFF_ZIP;
    }

    if ((tif = fopenTiff(fp, modestr)) == NULL)
        return ERROR_INT("tif not opened", procName, 1);

    if (pixWriteToTiffStream(tif, pix, comptype, NULL, NULL, NULL, NULL)) {
        TIFFCleanup(tif);
        return ERROR_INT("tif write error", procName, 1);
    }

    TIFFCleanup(tif);
    return 0;
}