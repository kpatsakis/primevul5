memstreamCreateForWrite(l_uint8  **poutdata,
                        size_t    *poutsize)
{
L_MEMSTREAM  *mstream;

    mstream = (L_MEMSTREAM *)LEPT_CALLOC(1, sizeof(L_MEMSTREAM));
    mstream->buffer = (l_uint8 *)LEPT_CALLOC(8 * 1024, 1);
    mstream->bufsize = 8 * 1024;
    mstream->poutdata = poutdata;  /* used only at end of write */
    mstream->poutsize = poutsize;  /* ditto  */
    mstream->hw = mstream->offset = 0;
    return mstream;
}