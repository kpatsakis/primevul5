tiffCloseCallback(thandle_t  handle)
{
L_MEMSTREAM  *mstream;

    mstream = (L_MEMSTREAM *)handle;
    if (mstream->poutdata) {   /* writing: save the output data */
        *mstream->poutdata = mstream->buffer;
        *mstream->poutsize = mstream->hw;
    }
    LEPT_FREE(mstream);  /* never free the buffer! */
    return 0;
}