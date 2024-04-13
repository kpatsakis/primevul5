bcf1_t *bcf_copy(bcf1_t *dst, bcf1_t *src)
{
    bcf1_sync(src);

    bcf_clear(dst);
    dst->rid  = src->rid;
    dst->pos  = src->pos;
    dst->rlen = src->rlen;
    dst->qual = src->qual;
    dst->n_info = src->n_info; dst->n_allele = src->n_allele;
    dst->n_fmt = src->n_fmt; dst->n_sample = src->n_sample;

    if ( dst->shared.m < src->shared.l )
    {
        dst->shared.s = (char*) realloc(dst->shared.s, src->shared.l);
        dst->shared.m = src->shared.l;
    }
    dst->shared.l = src->shared.l;
    memcpy(dst->shared.s,src->shared.s,dst->shared.l);

    if ( dst->indiv.m < src->indiv.l )
    {
        dst->indiv.s = (char*) realloc(dst->indiv.s, src->indiv.l);
        dst->indiv.m = src->indiv.l;
    }
    dst->indiv.l = src->indiv.l;
    memcpy(dst->indiv.s,src->indiv.s,dst->indiv.l);

    return dst;
}