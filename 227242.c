int bcf_hdr_set(bcf_hdr_t *hdr, const char *fname)
{
    int i = 0, n = 0, save_errno;
    char **lines = hts_readlines(fname, &n);
    if ( !lines ) return 1;
    for (i=0; i<n-1; i++)
    {
        int k;
        bcf_hrec_t *hrec = bcf_hdr_parse_line(hdr,lines[i],&k);
        if (!hrec) goto fail;
        if (bcf_hdr_add_hrec(hdr, hrec) < 0) {
            bcf_hrec_destroy(hrec);
            goto fail;
        }
        free(lines[i]);
        lines[i] = NULL;
    }
    if (bcf_hdr_parse_sample_line(hdr, lines[n-1]) < 0) goto fail;
    if (bcf_hdr_sync(hdr) < 0) goto fail;
    free(lines[n-1]);
    free(lines);
    return 0;

 fail:
    save_errno = errno;
    for (; i < n; i++)
        free(lines[i]);
    free(lines);
    errno = save_errno;
    return 1;
}