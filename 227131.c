static inline int bcf1_sync_filter(bcf1_t *line, kstring_t *str)
{
    // typed vector of integers
    if ( line->d.n_flt ) {
        return bcf_enc_vint(str, line->d.n_flt, line->d.flt, -1);
    } else {
        return bcf_enc_vint(str, 0, 0, -1);
    }
}