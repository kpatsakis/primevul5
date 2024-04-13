pixWriteTiff(const char  *filename,
             PIX         *pix,
             l_int32      comptype,
             const char  *modestr)
{
    return pixWriteTiffCustom(filename, pix, comptype, modestr,
                              NULL, NULL, NULL, NULL);
}