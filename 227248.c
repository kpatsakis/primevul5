static inline int _bcf1_sync_alleles(const bcf_hdr_t *hdr, bcf1_t *line, int nals)
{
    line->d.shared_dirty |= BCF1_DIRTY_ALS;

    line->n_allele = nals;
    hts_expand(char*, line->n_allele, line->d.m_allele, line->d.allele);

    char *als = line->d.als;
    int n = 0;
    while (n<nals)
    {
        line->d.allele[n] = als;
        while ( *als ) als++;
        als++;
        n++;
    }

    // Update REF length. Note that END is 1-based while line->pos 0-based
    bcf_info_t *end_info = bcf_get_info(hdr,line,"END");
    if ( end_info )
    {
        if ( end_info->type==BCF_HT_INT && end_info->v1.i==bcf_int32_missing ) end_info = NULL;
        else if ( end_info->type==BCF_HT_LONG && end_info->v1.i==bcf_int64_missing ) end_info = NULL;
    }
    if ( end_info && end_info->v1.i > line->pos )
        line->rlen = end_info->v1.i - line->pos;
    else if ( nals > 0 )
        line->rlen = strlen(line->d.allele[0]);
    else
        line->rlen = 0;

    return 0;
}