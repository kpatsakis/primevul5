int bcf_update_alleles_str(const bcf_hdr_t *hdr, bcf1_t *line, const char *alleles_string)
{
    if ( !(line->unpacked & BCF_UN_STR) ) bcf_unpack(line, BCF_UN_STR);
    kstring_t tmp;
    tmp.l = 0; tmp.s = line->d.als; tmp.m = line->d.m_als;
    kputs(alleles_string, &tmp);
    line->d.als = tmp.s; line->d.m_als = tmp.m;

    int nals = 1;
    char *t = line->d.als;
    while (*t)
    {
        if ( *t==',' ) { *t = 0; nals++; }
        t++;
    }
    return _bcf1_sync_alleles(hdr, line, nals);
}