int bcf_get_variant_types(bcf1_t *rec)
{
    if ( rec->d.var_type==-1 ) bcf_set_variant_types(rec);
    return rec->d.var_type;
}