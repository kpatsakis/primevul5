int bcf_get_variant_type(bcf1_t *rec, int ith_allele)
{
    if ( rec->d.var_type==-1 ) bcf_set_variant_types(rec);
    return rec->d.var[ith_allele].type;
}