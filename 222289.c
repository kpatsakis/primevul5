pixReadTiff(const char  *filename,
            l_int32      n)
{
FILE  *fp;
PIX   *pix;

    PROCNAME("pixReadTiff");

    if (!filename)
        return (PIX *)ERROR_PTR("filename not defined", procName, NULL);

    if ((fp = fopenReadStream(filename)) == NULL)
        return (PIX *)ERROR_PTR("image file not found", procName, NULL);
    pix = pixReadStreamTiff(fp, n);
    fclose(fp);
    return pix;
}