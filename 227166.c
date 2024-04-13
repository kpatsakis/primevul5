static int idx_calc_n_lvls_ids(const bcf_hdr_t *h, int min_shift,
                               int starting_n_lvls, int *nids_out)
{
    int n_lvls, i, nids = 0;
    int64_t max_len = 0, s;

    for (i = 0; i < h->n[BCF_DT_CTG]; ++i)
    {
        if ( !h->id[BCF_DT_CTG][i].val ) continue;
        if ( max_len < h->id[BCF_DT_CTG][i].val->info[0] )
            max_len = h->id[BCF_DT_CTG][i].val->info[0];
        nids++;
    }
    if ( !max_len ) max_len = (1LL<<31) - 1;  // In case contig line is broken.
    max_len += 256;
    s = 1LL << (min_shift + starting_n_lvls * 3);
    for (n_lvls = starting_n_lvls; max_len > s; ++n_lvls, s <<= 3);

    if (nids_out) *nids_out = nids;
    return n_lvls;
}