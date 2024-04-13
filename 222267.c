writeMultipageTiffSA(SARRAY      *sa,
                     const char  *fileout)
{
char        *fname;
const char  *op;
l_int32      i, nfiles, firstfile, format;
PIX         *pix;

    PROCNAME("writeMultipageTiffSA");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);
    if (!fileout)
        return ERROR_INT("fileout not defined", procName, 1);

    nfiles = sarrayGetCount(sa);
    firstfile = TRUE;
    for (i = 0; i < nfiles; i++) {
        op = (firstfile) ? "w" : "a";
        fname = sarrayGetString(sa, i, L_NOCOPY);
        findFileFormat(fname, &format);
        if (format == IFF_UNKNOWN) {
            L_INFO("format of %s not known\n", procName, fname);
            continue;
        }

        if ((pix = pixRead(fname)) == NULL) {
            L_WARNING("pix not made for file: %s\n", procName, fname);
            continue;
        }
        if (pixGetDepth(pix) == 1)
            pixWriteTiff(fileout, pix, IFF_TIFF_G4, op);
        else
            pixWriteTiff(fileout, pix, IFF_TIFF_ZIP, op);
        firstfile = FALSE;
        pixDestroy(&pix);
    }

    return 0;
}