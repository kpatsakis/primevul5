int bcf_remove_filter(const bcf_hdr_t *hdr, bcf1_t *line, int flt_id, int pass)
{
    if ( !(line->unpacked & BCF_UN_FLT) ) bcf_unpack(line, BCF_UN_FLT);
    int i;
    for (i=0; i<line->d.n_flt; i++)
        if ( flt_id==line->d.flt[i] ) break;
    if ( i==line->d.n_flt ) return 0;   // the filter is not present
    line->d.shared_dirty |= BCF1_DIRTY_FLT;
    if ( i!=line->d.n_flt-1 ) memmove(line->d.flt+i,line->d.flt+i+1,(line->d.n_flt-i-1)*sizeof(*line->d.flt));
    line->d.n_flt--;
    if ( !line->d.n_flt && pass ) bcf_add_filter(hdr,line,0);
    return 0;
}