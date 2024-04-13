static inline int bcf1_sync_alleles(bcf1_t *line, kstring_t *str)
{
    // list of typed strings
    int i;
    for (i=0; i<line->n_allele; i++) {
        if (bcf_enc_vchar(str, strlen(line->d.allele[i]), line->d.allele[i]) < 0)
            return -1;
    }
    if ( !line->rlen && line->n_allele ) line->rlen = strlen(line->d.allele[0]);
    return 0;
}