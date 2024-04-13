int bcf_hdr_sync(bcf_hdr_t *h)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        vdict_t *d = (vdict_t*)h->dict[i];
        khint_t k;
        if ( h->n[i] < kh_size(d) )
        {
            bcf_idpair_t *new_idpair;
            // this should be true only for i=2, BCF_DT_SAMPLE
            new_idpair = (bcf_idpair_t*) realloc(h->id[i], kh_size(d)*sizeof(bcf_idpair_t));
            if (!new_idpair) return -1;
            h->n[i] = kh_size(d);
            h->id[i] = new_idpair;
        }
        for (k=kh_begin(d); k<kh_end(d); k++)
        {
            if (!kh_exist(d,k)) continue;
            h->id[i][kh_val(d,k).id].key = kh_key(d,k);
            h->id[i][kh_val(d,k).id].val = &kh_val(d,k);
        }
    }
    h->dirty = 0;
    return 0;
}