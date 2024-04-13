static inline int bcf1_sync_info(bcf1_t *line, kstring_t *str)
{
    // pairs of typed vectors
    int i, irm = -1, e = 0;
    for (i=0; i<line->n_info; i++)
    {
        bcf_info_t *info = &line->d.info[i];
        if ( !info->vptr )
        {
            // marked for removal
            if ( irm < 0 ) irm = i;
            continue;
        }
        e |= kputsn_(info->vptr - info->vptr_off, info->vptr_len + info->vptr_off, str) < 0;
        if ( irm >=0 )
        {
            bcf_info_t tmp = line->d.info[irm]; line->d.info[irm] = line->d.info[i]; line->d.info[i] = tmp;
            while ( irm<=i && line->d.info[irm].vptr ) irm++;
        }
    }
    if ( irm>=0 ) line->n_info = irm;
    return e == 0 ? 0 : -1;
}