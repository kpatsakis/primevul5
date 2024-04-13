pixaReadMemMultipageTiff(const l_uint8  *data,
                         size_t          size)
{
size_t  offset;
PIX    *pix;
PIXA   *pixa;

    PROCNAME("pixaReadMemMultipageTiff");

    if (!data)
        return (PIXA *)ERROR_PTR("data not defined", procName, NULL);

    offset = 0;
    pixa = pixaCreate(0);
    do {
        pix = pixReadMemFromMultipageTiff(data, size, &offset);
        pixaAddPix(pixa, pix, L_INSERT);
    } while (offset != 0);
    return pixa;
}