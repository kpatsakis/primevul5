static int vcf_parse_filter(kstring_t *str, const bcf_hdr_t *h, bcf1_t *v, char *p, char *q) {
    int i, n_flt = 1, max_n_flt = 0;
    char *r, *t;
    int32_t *a_flt = NULL;
    ks_tokaux_t aux1;
    khint_t k;
    vdict_t *d = (vdict_t*)h->dict[BCF_DT_ID];
    // count the number of filters
    if (*(q-1) == ';') *(q-1) = 0;
    for (r = p; *r; ++r)
        if (*r == ';') ++n_flt;
    if (n_flt > max_n_flt) {
        a_flt = malloc(n_flt * sizeof(*a_flt));
        if (!a_flt) {
            hts_log_error("Could not allocate memory at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_LIMITS; // No appropriate code?
            return -1;
        }
        max_n_flt = n_flt;
    }
    // add filters
    for (t = kstrtok(p, ";", &aux1), i = 0; t; t = kstrtok(0, 0, &aux1)) {
        *(char*)aux1.p = 0;
        k = kh_get(vdict, d, t);
        if (k == kh_end(d))
        {
            // Simple error recovery for FILTERs not defined in the header. It will not help when VCF header has
            // been already printed, but will enable tools like vcfcheck to proceed.
            hts_log_warning("FILTER '%s' is not defined in the header", t);
            kstring_t tmp = {0,0,0};
            int l;
            ksprintf(&tmp, "##FILTER=<ID=%s,Description=\"Dummy\">", t);
            bcf_hrec_t *hrec = bcf_hdr_parse_line(h,tmp.s,&l);
            free(tmp.s);
            int res = hrec ? bcf_hdr_add_hrec((bcf_hdr_t*)h, hrec) : -1;
            if (res < 0) bcf_hrec_destroy(hrec);
            if (res > 0) res = bcf_hdr_sync((bcf_hdr_t*)h);
            k = kh_get(vdict, d, t);
            v->errcode |= BCF_ERR_TAG_UNDEF;
            if (res || k == kh_end(d)) {
                hts_log_error("Could not add dummy header for FILTER '%s' at %s:%"PRIhts_pos, t, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_TAG_INVALID;
                free(a_flt);
                return -1;
            }
        }
        a_flt[i++] = kh_val(d, k).id;
    }

    bcf_enc_vint(str, n_flt, a_flt, -1);
    free(a_flt);

    return 0;
}