void bcf_empty(bcf1_t *v)
{
    bcf_clear1(v);
    free(v->d.id);
    free(v->d.als);
    free(v->d.allele); free(v->d.flt); free(v->d.info); free(v->d.fmt);
    if (v->d.var ) free(v->d.var);
    free(v->shared.s); free(v->indiv.s);
    memset(&v->d,0,sizeof(v->d));
    memset(&v->shared,0,sizeof(v->shared));
    memset(&v->indiv,0,sizeof(v->indiv));
}