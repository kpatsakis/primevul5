static int bcf1_sync(bcf1_t *line)
{
    char *shared_ori = line->shared.s;
    size_t prev_len;

    kstring_t tmp = {0,0,0};
    if ( !line->shared.l )
    {
        // New line created via API, BCF data blocks do not exist. Get it ready for BCF output
        tmp = line->shared;
        bcf1_sync_id(line, &tmp);
        line->unpack_size[0] = tmp.l; prev_len = tmp.l;

        bcf1_sync_alleles(line, &tmp);
        line->unpack_size[1] = tmp.l - prev_len; prev_len = tmp.l;

        bcf1_sync_filter(line, &tmp);
        line->unpack_size[2] = tmp.l - prev_len;

        bcf1_sync_info(line, &tmp);
        line->shared = tmp;
    }
    else if ( line->d.shared_dirty )
    {
        // The line was edited, update the BCF data block.

        if ( !(line->unpacked & BCF_UN_STR) ) bcf_unpack(line,BCF_UN_STR);

        // ptr_ori points to the original unchanged BCF data.
        uint8_t *ptr_ori = (uint8_t *) line->shared.s;

        // ID: single typed string
        if ( line->d.shared_dirty & BCF1_DIRTY_ID )
            bcf1_sync_id(line, &tmp);
        else
            kputsn_(ptr_ori, line->unpack_size[0], &tmp);
        ptr_ori += line->unpack_size[0];
        line->unpack_size[0] = tmp.l; prev_len = tmp.l;

        // REF+ALT: list of typed strings
        if ( line->d.shared_dirty & BCF1_DIRTY_ALS )
            bcf1_sync_alleles(line, &tmp);
        else
        {
            kputsn_(ptr_ori, line->unpack_size[1], &tmp);
            if ( !line->rlen && line->n_allele ) line->rlen = strlen(line->d.allele[0]);
        }
        ptr_ori += line->unpack_size[1];
        line->unpack_size[1] = tmp.l - prev_len; prev_len = tmp.l;

        if ( line->unpacked & BCF_UN_FLT )
        {
            // FILTER: typed vector of integers
            if ( line->d.shared_dirty & BCF1_DIRTY_FLT )
                bcf1_sync_filter(line, &tmp);
            else if ( line->d.n_flt )
                kputsn_(ptr_ori, line->unpack_size[2], &tmp);
            else
                bcf_enc_vint(&tmp, 0, 0, -1);
            ptr_ori += line->unpack_size[2];
            line->unpack_size[2] = tmp.l - prev_len;

            if ( line->unpacked & BCF_UN_INFO )
            {
                // INFO: pairs of typed vectors
                if ( line->d.shared_dirty & BCF1_DIRTY_INF )
                {
                    bcf1_sync_info(line, &tmp);
                    ptr_ori = (uint8_t*)line->shared.s + line->shared.l;
                }
            }
        }

        int size = line->shared.l - (size_t)ptr_ori + (size_t)line->shared.s;
        if ( size ) kputsn_(ptr_ori, size, &tmp);

        free(line->shared.s);
        line->shared = tmp;
    }
    if ( line->shared.s != shared_ori && line->unpacked & BCF_UN_INFO )
    {
        // Reallocated line->shared.s block invalidated line->d.info[].vptr pointers
        size_t off_new = line->unpack_size[0] + line->unpack_size[1] + line->unpack_size[2];
        int i;
        for (i=0; i<line->n_info; i++)
        {
            uint8_t *vptr_free = line->d.info[i].vptr_free ? line->d.info[i].vptr - line->d.info[i].vptr_off : NULL;
            line->d.info[i].vptr = (uint8_t*) line->shared.s + off_new + line->d.info[i].vptr_off;
            off_new += line->d.info[i].vptr_len + line->d.info[i].vptr_off;
            if ( vptr_free )
            {
                free(vptr_free);
                line->d.info[i].vptr_free = 0;
            }
        }
    }

    if ( line->n_sample && line->n_fmt && (!line->indiv.l || line->d.indiv_dirty) )
    {
        // The genotype fields changed or are not present
        tmp.l = tmp.m = 0; tmp.s = NULL;
        int i, irm = -1;
        for (i=0; i<line->n_fmt; i++)
        {
            bcf_fmt_t *fmt = &line->d.fmt[i];
            if ( !fmt->p )
            {
                // marked for removal
                if ( irm < 0 ) irm = i;
                continue;
            }
            kputsn_(fmt->p - fmt->p_off, fmt->p_len + fmt->p_off, &tmp);
            if ( irm >=0 )
            {
                bcf_fmt_t tfmt = line->d.fmt[irm]; line->d.fmt[irm] = line->d.fmt[i]; line->d.fmt[i] = tfmt;
                while ( irm<=i && line->d.fmt[irm].p ) irm++;
            }

        }
        if ( irm>=0 ) line->n_fmt = irm;
        free(line->indiv.s);
        line->indiv = tmp;

        // Reallocated line->indiv.s block invalidated line->d.fmt[].p pointers
        size_t off_new = 0;
        for (i=0; i<line->n_fmt; i++)
        {
            uint8_t *p_free = line->d.fmt[i].p_free ? line->d.fmt[i].p - line->d.fmt[i].p_off : NULL;
            line->d.fmt[i].p = (uint8_t*) line->indiv.s + off_new + line->d.fmt[i].p_off;
            off_new += line->d.fmt[i].p_len + line->d.fmt[i].p_off;
            if ( p_free )
            {
                free(p_free);
                line->d.fmt[i].p_free = 0;
            }
        }
    }
    if ( !line->n_sample ) line->n_fmt = 0;
    line->d.shared_dirty = line->d.indiv_dirty = 0;
    return 0;
}