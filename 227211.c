void bcf_destroy(bcf1_t *v)
{
    if (!v) return;
    bcf_empty1(v);
    free(v);
}