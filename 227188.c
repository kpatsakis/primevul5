int bcf_index_build2(const char *fn, const char *fnidx, int min_shift)
{
    return bcf_index_build3(fn, fnidx, min_shift, 0);
}