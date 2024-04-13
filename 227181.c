bcf_fmt_t *bcf_get_fmt_id(bcf1_t *line, const int id)
{
    int i;
    if ( !(line->unpacked & BCF_UN_FMT) ) bcf_unpack(line, BCF_UN_FMT);
    for (i=0; i<line->n_fmt; i++)
    {
        if ( line->d.fmt[i].id==id ) return &line->d.fmt[i];
    }
    return NULL;
}