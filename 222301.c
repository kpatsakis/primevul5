tiffSeekCallback(thandle_t  handle,
                 toff_t     offset,
                 l_int32    whence)
{
L_MEMSTREAM  *mstream;

    PROCNAME("tiffSeekCallback");
    mstream = (L_MEMSTREAM *)handle;
    switch (whence) {
        case SEEK_SET:
/*            lept_stderr("seek_set: offset = %d\n", offset); */
            if((size_t)offset != offset) {  /* size_t overflow on uint32 */
                return (toff_t)ERROR_INT("too large offset value", procName, 1);
            }
            mstream->offset = offset;
            break;
        case SEEK_CUR:
/*            lept_stderr("seek_cur: offset = %d\n", offset); */
            mstream->offset += offset;
            break;
        case SEEK_END:
/*            lept_stderr("seek end: hw = %d, offset = %d\n",
                    mstream->hw, offset); */
            mstream->offset = mstream->hw - offset;  /* offset >= 0 */
            break;
        default:
            return (toff_t)ERROR_INT("bad whence value", procName,
                                     mstream->offset);
    }

    return mstream->offset;
}