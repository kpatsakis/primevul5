bcf_hdr_t *bcf_hdr_init(const char *mode)
{
    int i;
    bcf_hdr_t *h;
    h = (bcf_hdr_t*)calloc(1, sizeof(bcf_hdr_t));
    if (!h) return NULL;
    for (i = 0; i < 3; ++i)
        if ((h->dict[i] = kh_init(vdict)) == NULL) goto fail;
    if ( strchr(mode,'w') )
    {
        bcf_hdr_append(h, "##fileformat=VCFv4.2");
        // The filter PASS must appear first in the dictionary
        bcf_hdr_append(h, "##FILTER=<ID=PASS,Description=\"All filters passed\">");
    }
    return h;

 fail:
    for (i = 0; i < 3; ++i)
        kh_destroy(vdict, h->dict[i]);
    free(h);
    return NULL;
}