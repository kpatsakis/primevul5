static int add_missing_contig_hrec(bcf_hdr_t *h, const char *name) {
    bcf_hrec_t *hrec = calloc(1, sizeof(bcf_hrec_t));
    int save_errno;
    if (!hrec) goto fail;

    hrec->key = strdup("contig");
    if (!hrec->key) goto fail;

    if (bcf_hrec_add_key(hrec, "ID", strlen("ID")) < 0) goto fail;
    if (bcf_hrec_set_val(hrec, hrec->nkeys-1, name, strlen(name), 0) < 0)
        goto fail;
    if (bcf_hdr_add_hrec(h, hrec) < 0)
        goto fail;
    return 0;

 fail:
    save_errno = errno;
    hts_log_error("%s", strerror(errno));
    if (hrec) bcf_hrec_destroy(hrec);
    errno = save_errno;
    return -1;
}