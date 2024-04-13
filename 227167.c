void bcf_hdr_destroy(bcf_hdr_t *h)
{
    int i;
    khint_t k;
    if (!h) return;
    for (i = 0; i < 3; ++i) {
        vdict_t *d = (vdict_t*)h->dict[i];
        if (d == 0) continue;
        for (k = kh_begin(d); k != kh_end(d); ++k)
            if (kh_exist(d, k)) free((char*)kh_key(d, k));
        kh_destroy(vdict, d);
        free(h->id[i]);
    }
    for (i=0; i<h->nhrec; i++)
        bcf_hrec_destroy(h->hrec[i]);
    if (h->nhrec) free(h->hrec);
    if (h->samples) free(h->samples);
    free(h->keep_samples);
    free(h->transl[0]); free(h->transl[1]);
    free(h->mem.s);
    free(h);
}