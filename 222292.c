memstreamCreateForRead(l_uint8  *indata,
                       size_t    insize)
{
L_MEMSTREAM  *mstream;

    mstream = (L_MEMSTREAM *)LEPT_CALLOC(1, sizeof(L_MEMSTREAM));
    mstream->buffer = indata;   /* handle to input data array */
    mstream->bufsize = insize;  /* amount of input data */
    mstream->hw = insize;       /* high-water mark fixed at input data size */
    mstream->offset = 0;        /* offset always starts at 0 */
    return mstream;
}