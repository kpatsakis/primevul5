hts_idx_t *bcf_index_load2(const char *fn, const char *fnidx)
{
    return fnidx? hts_idx_load2(fn, fnidx) : bcf_index_load(fn);
}