int bcf_update_id(const bcf_hdr_t *hdr, bcf1_t *line, const char *id)
{
    if ( !(line->unpacked & BCF_UN_STR) ) bcf_unpack(line, BCF_UN_STR);
    kstring_t tmp;
    tmp.l = 0; tmp.s = line->d.id; tmp.m = line->d.m_id;
    if ( id )
        kputs(id, &tmp);
    else
        kputs(".", &tmp);
    line->d.id = tmp.s; line->d.m_id = tmp.m;
    line->d.shared_dirty |= BCF1_DIRTY_ID;
    return 0;
}