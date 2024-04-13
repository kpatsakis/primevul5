int bcf_subset(const bcf_hdr_t *h, bcf1_t *v, int n, int *imap)
{
    kstring_t ind;
    ind.s = 0; ind.l = ind.m = 0;
    if (n) {
        bcf_fmt_t fmt[MAX_N_FMT];
        int i, j;
        uint8_t *ptr = (uint8_t*)v->indiv.s;
        for (i = 0; i < v->n_fmt; ++i)
            ptr = bcf_unpack_fmt_core1(ptr, v->n_sample, &fmt[i]);
        for (i = 0; i < (int)v->n_fmt; ++i) {
            bcf_fmt_t *f = &fmt[i];
            bcf_enc_int1(&ind, f->id);
            bcf_enc_size(&ind, f->n, f->type);
            for (j = 0; j < n; ++j)
                if (imap[j] >= 0) kputsn((char*)(f->p + imap[j] * f->size), f->size, &ind);
        }
        for (i = j = 0; j < n; ++j) if (imap[j] >= 0) ++i;
        v->n_sample = i;
    } else v->n_sample = 0;
    if ( !v->n_sample ) v->n_fmt = 0;
    free(v->indiv.s);
    v->indiv = ind;
    v->unpacked &= ~BCF_UN_FMT;    // only BCF is ready for output, VCF will need to unpack again
    return 0;
}