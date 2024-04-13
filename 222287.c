pixWriteMemTiff(l_uint8  **pdata,
                size_t    *psize,
                PIX       *pix,
                l_int32    comptype)
{
    return pixWriteMemTiffCustom(pdata, psize, pix, comptype,
                                 NULL, NULL, NULL, NULL);
}