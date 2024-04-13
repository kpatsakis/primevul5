pixWriteStreamTiff(FILE    *fp,
                   PIX     *pix,
                   l_int32  comptype)
{
    return pixWriteStreamTiffWA(fp, pix, comptype, "w");
}