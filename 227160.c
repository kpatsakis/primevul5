bcf_hdr_t *bcf_hdr_dup(const bcf_hdr_t *hdr)
{
    bcf_hdr_t *hout = bcf_hdr_init("r");
    if (!hout) {
        hts_log_error("Failed to allocate bcf header");
        return NULL;
    }
    kstring_t htxt = {0,0,0};
    bcf_hdr_format(hdr, 1, &htxt);
    if ( bcf_hdr_parse(hout, htxt.s) < 0 ) {
        bcf_hdr_destroy(hout);
        hout = NULL;
    }
    free(htxt.s);
    return hout;
}