static khint_t fix_chromosome(const bcf_hdr_t *h, vdict_t *d, const char *p) {
    // Simple error recovery for chromosomes not defined in the header. It will not help when VCF header has
    // been already printed, but will enable tools like vcfcheck to proceed.

    kstring_t tmp = {0,0,0};
    khint_t k;
    int l;
    ksprintf(&tmp, "##contig=<ID=%s>", p);
    bcf_hrec_t *hrec = bcf_hdr_parse_line(h,tmp.s,&l);
    free(tmp.s);
    int res = hrec ? bcf_hdr_add_hrec((bcf_hdr_t*)h, hrec) : -1;
    if (res < 0) bcf_hrec_destroy(hrec);
    if (res > 0) res = bcf_hdr_sync((bcf_hdr_t*)h);
    k = kh_get(vdict, d, p);

    return k;
}