pixaReadMultipageTiff(const char  *filename)
{
l_int32  i, npages;
FILE    *fp;
PIX     *pix;
PIXA    *pixa;
TIFF    *tif;

    PROCNAME("pixaReadMultipageTiff");

    if (!filename)
        return (PIXA *)ERROR_PTR("filename not defined", procName, NULL);

    if ((fp = fopenReadStream(filename)) == NULL)
        return (PIXA *)ERROR_PTR("stream not opened", procName, NULL);
    if (fileFormatIsTiff(fp)) {
        tiffGetCount(fp, &npages);
        L_INFO(" Tiff: %d pages\n", procName, npages);
    } else {
        return (PIXA *)ERROR_PTR("file not tiff", procName, NULL);
    }

    if ((tif = fopenTiff(fp, "r")) == NULL)
        return (PIXA *)ERROR_PTR("tif not opened", procName, NULL);

    pixa = pixaCreate(npages);
    pix = NULL;
    for (i = 0; i < npages; i++) {
        if ((pix = pixReadFromTiffStream(tif)) != NULL) {
            pixaAddPix(pixa, pix, L_INSERT);
        } else {
            L_WARNING("pix not read for page %d\n", procName, i);
        }

            /* Advance to the next directory (i.e., the next image) */
        if (TIFFReadDirectory(tif) == 0)
            break;
    }

    fclose(fp);
    TIFFCleanup(tif);
    return pixa;
}