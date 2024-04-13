bcf_hdr_t *bcf_hdr_subset(const bcf_hdr_t *h0, int n, char *const* samples, int *imap)
{
    void *names_hash = khash_str2int_init();
    kstring_t htxt = {0,0,0};
    kstring_t str = {0,0,0};
    bcf_hdr_t *h = bcf_hdr_init("w");
    if (!h) {
        hts_log_error("Failed to allocate bcf header");
        khash_str2int_destroy(names_hash);
        return NULL;
    }
    bcf_hdr_format(h0, 1, &htxt);
    bcf_hdr_set_version(h,bcf_hdr_get_version(h0));
    int j;
    for (j=0; j<n; j++) imap[j] = -1;
    if ( bcf_hdr_nsamples(h0) > 0) {
        char *p = find_chrom_header_line(htxt.s);
        int i = 0, end = n? 8 : 7;
        while ((p = strchr(p, '\t')) != 0 && i < end) ++i, ++p;
        if (i != end) {
            free(h); free(str.s);
            return 0; // malformated header
        }
        kputsn(htxt.s, p - htxt.s, &str);
        for (i = 0; i < n; ++i) {
            if ( khash_str2int_has_key(names_hash,samples[i]) )
            {
                hts_log_error("Duplicate sample name \"%s\"", samples[i]);
                free(str.s);
                free(htxt.s);
                khash_str2int_destroy(names_hash);
                bcf_hdr_destroy(h);
                return NULL;
            }
            imap[i] = bcf_hdr_id2int(h0, BCF_DT_SAMPLE, samples[i]);
            if (imap[i] < 0) continue;
            kputc('\t', &str);
            kputs(samples[i], &str);
            khash_str2int_inc(names_hash,samples[i]);
        }
    } else kputsn(htxt.s, htxt.l, &str);
    while (str.l && (!str.s[str.l-1] || str.s[str.l-1]=='\n') ) str.l--; // kill trailing zeros and newlines
    kputc('\n',&str);
    if ( bcf_hdr_parse(h, str.s) < 0 ) {
        bcf_hdr_destroy(h);
        h = NULL;
    }
    free(str.s);
    free(htxt.s);
    khash_str2int_destroy(names_hash);
    return h;
}