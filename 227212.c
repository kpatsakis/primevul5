bcf_hrec_t *bcf_hrec_dup(bcf_hrec_t *hrec)
{
    int save_errno;
    bcf_hrec_t *out = (bcf_hrec_t*) calloc(1,sizeof(bcf_hrec_t));
    if (!out) return NULL;

    out->type = hrec->type;
    if ( hrec->key ) {
        out->key = strdup(hrec->key);
        if (!out->key) goto fail;
    }
    if ( hrec->value ) {
        out->value = strdup(hrec->value);
        if (!out->value) goto fail;
    }
    out->nkeys = hrec->nkeys;
    out->keys = (char**) malloc(sizeof(char*)*hrec->nkeys);
    if (!out->keys) goto fail;
    out->vals = (char**) malloc(sizeof(char*)*hrec->nkeys);
    if (!out->vals) goto fail;
    int i, j = 0;
    for (i=0; i<hrec->nkeys; i++)
    {
        if ( hrec->keys[i] && !strcmp("IDX",hrec->keys[i]) ) continue;
        if ( hrec->keys[i] ) {
            out->keys[j] = strdup(hrec->keys[i]);
            if (!out->keys[j]) goto fail;
        }
        if ( hrec->vals[i] ) {
            out->vals[j] = strdup(hrec->vals[i]);
            if (!out->vals[j]) goto fail;
        }
        j++;
    }
    if ( i!=j ) out->nkeys -= i-j;   // IDX was omitted
    return out;

 fail:
    save_errno = errno;
    hts_log_error("%s", strerror(errno));
    bcf_hrec_destroy(out);
    errno = save_errno;
    return NULL;
}