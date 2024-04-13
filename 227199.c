bcf1_t *bcf_dup(bcf1_t *src)
{
    bcf1_t *out = bcf_init1();
    return bcf_copy(out, src);
}