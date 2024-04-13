tiffSizeCallback(thandle_t  handle)
{
L_MEMSTREAM  *mstream;

    mstream = (L_MEMSTREAM *)handle;
    return mstream->hw;
}