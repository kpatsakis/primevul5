void bcf_clear(bcf1_t *v)
{
    int i;
    for (i=0; i<v->d.m_info; i++)
    {
        if ( v->d.info[i].vptr_free )
        {
            free(v->d.info[i].vptr - v->d.info[i].vptr_off);
            v->d.info[i].vptr_free = 0;
        }
    }
    for (i=0; i<v->d.m_fmt; i++)
    {
        if ( v->d.fmt[i].p_free )
        {
            free(v->d.fmt[i].p - v->d.fmt[i].p_off);
            v->d.fmt[i].p_free = 0;
        }
    }
    v->rid = v->pos = v->rlen = v->unpacked = 0;
    bcf_float_set_missing(v->qual);
    v->n_info = v->n_allele = v->n_fmt = v->n_sample = 0;
    v->shared.l = v->indiv.l = 0;
    v->d.var_type = -1;
    v->d.shared_dirty = 0;
    v->d.indiv_dirty  = 0;
    v->d.n_flt = 0;
    v->errcode = 0;
    if (v->d.m_als) v->d.als[0] = 0;
    if (v->d.m_id) v->d.id[0] = 0;
}