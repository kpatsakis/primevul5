hts_idx_t *bcf_index_load3(const char *fn, const char *fnidx, int flags)
{
    return hts_idx_load3(fn, fnidx, HTS_FMT_CSI, flags);
}