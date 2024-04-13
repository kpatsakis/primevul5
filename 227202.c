static int bcf_set_variant_types(bcf1_t *b)
{
    if ( !(b->unpacked & BCF_UN_STR) ) bcf_unpack(b, BCF_UN_STR);
    bcf_dec_t *d = &b->d;
    if ( d->n_var < b->n_allele )
    {
        d->var = (variant_t *) realloc(d->var, sizeof(variant_t)*b->n_allele);
        d->n_var = b->n_allele;
    }
    int i;
    b->d.var_type = 0;
    d->var[0].type = VCF_REF;
    d->var[0].n    = 0;
    for (i=1; i<b->n_allele; i++)
    {
        bcf_set_variant_type(d->allele[0],d->allele[i], &d->var[i]);
        b->d.var_type |= d->var[i].type;
        //fprintf(stderr,"[set_variant_type] %d   %s %s -> %d %d .. %d\n", b->pos+1,d->allele[0],d->allele[i],d->var[i].type,d->var[i].n, b->d.var_type);
    }
    return 0;
}