int bcf_index_build(const char *fn, int min_shift)
{
    return bcf_index_build3(fn, NULL, min_shift, 0);
}