int bcf_update_filter(const bcf_hdr_t *hdr, bcf1_t *line, int *flt_ids, int n)
{
    if ( !(line->unpacked & BCF_UN_FLT) ) bcf_unpack(line, BCF_UN_FLT);
    line->d.shared_dirty |= BCF1_DIRTY_FLT;
    line->d.n_flt = n;
    if ( !n ) return 0;
    hts_expand(int, line->d.n_flt, line->d.m_flt, line->d.flt);
    int i;
    for (i=0; i<n; i++)
        line->d.flt[i] = flt_ids[i];
    return 0;
}