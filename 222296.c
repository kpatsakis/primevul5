pixWriteMemTiffCustom(l_uint8  **pdata,
                      size_t    *psize,
                      PIX       *pix,
                      l_int32    comptype,
                      NUMA      *natags,
                      SARRAY    *savals,
                      SARRAY    *satypes,
                      NUMA      *nasizes)
{
l_int32  ret;
TIFF    *tif;

    PROCNAME("pixWriteMemTiffCustom");

    if (!pdata)
        return ERROR_INT("&data not defined", procName, 1);
    if (!psize)
        return ERROR_INT("&size not defined", procName, 1);
    if (!pix)
        return ERROR_INT("&pix not defined", procName, 1);
    if (pixGetDepth(pix) != 1 && comptype != IFF_TIFF &&
        comptype != IFF_TIFF_LZW && comptype != IFF_TIFF_ZIP &&
        comptype != IFF_TIFF_JPEG) {
        L_WARNING("invalid compression type for bpp > 1\n", procName);
        comptype = IFF_TIFF_ZIP;
    }

    if ((tif = fopenTiffMemstream("tifferror", "w", pdata, psize)) == NULL)
        return ERROR_INT("tiff stream not opened", procName, 1);
    ret = pixWriteToTiffStream(tif, pix, comptype, natags, savals,
                               satypes, nasizes);

    TIFFClose(tif);
    return ret;
}