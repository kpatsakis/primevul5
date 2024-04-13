int bcf_update_alleles(const bcf_hdr_t *hdr, bcf1_t *line, const char **alleles, int nals)
{
    if ( !(line->unpacked & BCF_UN_STR) ) bcf_unpack(line, BCF_UN_STR);
    kstring_t tmp = {0,0,0};
    char *free_old = NULL;

    // If the supplied alleles are not pointers to line->d.als, the existing block can be reused.
    int i;
    for (i=0; i<nals; i++)
        if ( alleles[i]>=line->d.als && alleles[i]<line->d.als+line->d.m_als ) break;
    if ( i==nals )
    {
        // all alleles point elsewhere, reuse the existing block
        tmp.l = 0; tmp.s = line->d.als; tmp.m = line->d.m_als;
    }
    else
        free_old = line->d.als;

    for (i=0; i<nals; i++)
    {
        kputs(alleles[i], &tmp);
        kputc(0, &tmp);
    }
    line->d.als = tmp.s; line->d.m_als = tmp.m;
    free(free_old);
    return _bcf1_sync_alleles(hdr,line,nals);
}