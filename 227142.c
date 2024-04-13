int bcf_add_filter(const bcf_hdr_t *hdr, bcf1_t *line, int flt_id)
{
    if ( !(line->unpacked & BCF_UN_FLT) ) bcf_unpack(line, BCF_UN_FLT);
    int i;
    for (i=0; i<line->d.n_flt; i++)
        if ( flt_id==line->d.flt[i] ) break;
    if ( i<line->d.n_flt ) return 0;    // this filter is already set
    line->d.shared_dirty |= BCF1_DIRTY_FLT;
    if ( flt_id==0 )    // set to PASS
        line->d.n_flt = 1;
    else if ( line->d.n_flt==1 && line->d.flt[0]==0 )
        line->d.n_flt = 1;
    else
        line->d.n_flt++;
    hts_expand(int, line->d.n_flt, line->d.m_flt, line->d.flt);
    line->d.flt[line->d.n_flt-1] = flt_id;
    return 1;
}