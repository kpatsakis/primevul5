int bcf_add_id(const bcf_hdr_t *hdr, bcf1_t *line, const char *id)
{
    if ( !id ) return 0;
    if ( !(line->unpacked & BCF_UN_STR) ) bcf_unpack(line, BCF_UN_STR);

    kstring_t tmp;
    tmp.l = 0; tmp.s = line->d.id; tmp.m = line->d.m_id;

    int len = strlen(id);
    char *dst = line->d.id;
    while ( *dst && (dst=strstr(dst,id)) )
    {
        if ( dst[len]!=0 && dst[len]!=';' ) dst++;              // a prefix, not a match
        else if ( dst==line->d.id || dst[-1]==';' ) return 0;   // already present
        dst++;  // a suffix, not a match
    }
    if ( line->d.id && (line->d.id[0]!='.' || line->d.id[1]) )
    {
        tmp.l = strlen(line->d.id);
        kputc(';',&tmp);
    }
    kputs(id,&tmp);

    line->d.id = tmp.s; line->d.m_id = tmp.m;
    line->d.shared_dirty |= BCF1_DIRTY_ID;
    return 0;

}