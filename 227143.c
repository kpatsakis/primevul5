int bcf_hrec_add_key(bcf_hrec_t *hrec, const char *str, size_t len)
{
    char **tmp;
    size_t n = hrec->nkeys + 1;
    assert(len > 0 && len < SIZE_MAX);
    tmp = realloc(hrec->keys, sizeof(char*)*n);
    if (!tmp) return -1;
    hrec->keys = tmp;
    tmp = realloc(hrec->vals, sizeof(char*)*n);
    if (!tmp) return -1;
    hrec->vals = tmp;

    hrec->keys[hrec->nkeys] = (char*) malloc((len+1)*sizeof(char));
    if (!hrec->keys[hrec->nkeys]) return -1;
    memcpy(hrec->keys[hrec->nkeys],str,len);
    hrec->keys[hrec->nkeys][len] = 0;
    hrec->vals[hrec->nkeys] = NULL;
    hrec->nkeys = n;
    return 0;
}