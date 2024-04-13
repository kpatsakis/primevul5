bcf_info_t *bcf_get_info_id(bcf1_t *line, const int id)
{
    int i;
    if ( !(line->unpacked & BCF_UN_INFO) ) bcf_unpack(line, BCF_UN_INFO);
    for (i=0; i<line->n_info; i++)
    {
        if ( line->d.info[i].key==id ) return &line->d.info[i];
    }
    return NULL;
}