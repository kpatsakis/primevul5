int bcf_hdr_add_sample(bcf_hdr_t *h, const char *s)
{
    return bcf_hdr_add_sample_len(h, s, 0);
}