static int bcf_hdr_add_sample_len(bcf_hdr_t *h, const char *s, size_t len)
{
    if ( !s ) return 0;
    if (len == 0) len = strlen(s);

    const char *ss = s;
    while ( *ss && isspace_c(*ss) && ss - s < len) ss++;
    if ( !*ss || ss - s == len)
    {
        hts_log_error("Empty sample name: trailing spaces/tabs in the header line?");
        return -1;
    }

    vdict_t *d = (vdict_t*)h->dict[BCF_DT_SAMPLE];
    int ret;
    char *sdup = malloc(len + 1);
    if (!sdup) return -1;
    memcpy(sdup, s, len);
    sdup[len] = 0;

    // Ensure space is available in h->samples
    size_t n = kh_size(d);
    char **new_samples = realloc(h->samples, sizeof(char*) * (n + 1));
    if (!new_samples) {
        free(sdup);
        return -1;
    }
    h->samples = new_samples;

    int k = kh_put(vdict, d, sdup, &ret);
    if (ret < 0) {
        free(sdup);
        return -1;
    }
    if (ret) { // absent
        kh_val(d, k) = bcf_idinfo_def;
        kh_val(d, k).id = n;
    } else {
        hts_log_error("Duplicated sample name '%s'", s);
        free(sdup);
        return -1;
    }
    h->samples[n] = sdup;
    h->dirty = 1;
    return 0;
}