int hrec_add_idx(bcf_hrec_t *hrec, int idx)
{
    int n = hrec->nkeys + 1;
    char **tmp = (char**) realloc(hrec->keys, sizeof(char*)*n);
    if (!tmp) return -1;
    hrec->keys = tmp;

    tmp = (char**) realloc(hrec->vals, sizeof(char*)*n);
    if (!tmp) return -1;
    hrec->vals = tmp;

    hrec->keys[hrec->nkeys] = strdup("IDX");
    if (!hrec->keys[hrec->nkeys]) return -1;

    kstring_t str = {0,0,0};
    if (kputw(idx, &str) < 0) {
        free(hrec->keys[hrec->nkeys]);
        return -1;
    }
    hrec->vals[hrec->nkeys] = str.s;
    hrec->nkeys = n;
    return 0;
}