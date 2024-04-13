tiffMapCallback(thandle_t  handle,
                tdata_t   *data,
                toff_t    *length)
{
L_MEMSTREAM  *mstream;

    mstream = (L_MEMSTREAM *)handle;
    *data = mstream->buffer;
    *length = mstream->hw;
    return 0;
}