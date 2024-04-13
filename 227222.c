int bcf_update_format(const bcf_hdr_t *hdr, bcf1_t *line, const char *key, const void *values, int n, int type)
{
    // Is the field already present?
    int i, fmt_id = bcf_hdr_id2int(hdr,BCF_DT_ID,key);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_FMT,fmt_id) )
    {
        if ( !n ) return 0;
        return -1;  // the key not present in the header
    }

    if ( !(line->unpacked & BCF_UN_FMT) ) bcf_unpack(line, BCF_UN_FMT);

    for (i=0; i<line->n_fmt; i++)
        if ( line->d.fmt[i].id==fmt_id ) break;
    bcf_fmt_t *fmt = i==line->n_fmt ? NULL : &line->d.fmt[i];

    if ( !n )
    {
        if ( fmt )
        {
            // Mark the tag for removal, free existing memory if necessary
            if ( fmt->p_free )
            {
                free(fmt->p - fmt->p_off);
                fmt->p_free = 0;
            }
            line->d.indiv_dirty = 1;
            fmt->p = NULL;
        }
        return 0;
    }

    line->n_sample = bcf_hdr_nsamples(hdr);
    int nps = n / line->n_sample;  // number of values per sample
    assert( nps && nps*line->n_sample==n );     // must be divisible by n_sample

    // Encode the values and determine the size required to accommodate the values
    kstring_t str = {0,0,0};
    bcf_enc_int1(&str, fmt_id);
    assert(values != NULL);
    if ( type==BCF_HT_INT )
        bcf_enc_vint(&str, n, (int32_t*)values, nps);
    else if ( type==BCF_HT_REAL )
    {
        bcf_enc_size(&str, nps, BCF_BT_FLOAT);
        serialize_float_array(&str, nps*line->n_sample, (float *) values);
    }
    else if ( type==BCF_HT_STR )
    {
        bcf_enc_size(&str, nps, BCF_BT_CHAR);
        kputsn((char*)values, nps*line->n_sample, &str);
    }
    else
    {
        hts_log_error("The type %d not implemented yet at %s:%"PRIhts_pos, type, bcf_seqname_safe(hdr,line), line->pos+1);
        abort();
    }

    if ( !fmt )
    {
        // Not present, new format field
        line->n_fmt++;
        hts_expand0(bcf_fmt_t, line->n_fmt, line->d.m_fmt, line->d.fmt);

        // Special case: VCF specification requires that GT is always first
        if ( line->n_fmt > 1 && key[0]=='G' && key[1]=='T' && !key[2] )
        {
            for (i=line->n_fmt-1; i>0; i--)
                line->d.fmt[i] = line->d.fmt[i-1];
            fmt = &line->d.fmt[0];
        }
        else
            fmt = &line->d.fmt[line->n_fmt-1];
        bcf_unpack_fmt_core1((uint8_t*)str.s, line->n_sample, fmt);
        line->d.indiv_dirty = 1;
        fmt->p_free = 1;
    }
    else
    {
        // The tag is already present, check if it is big enough to accomodate the new block
        if ( str.l <= fmt->p_len + fmt->p_off )
        {
            // good, the block is big enough
            if ( str.l != fmt->p_len + fmt->p_off ) line->d.indiv_dirty = 1;
            uint8_t *ptr = fmt->p - fmt->p_off;
            memcpy(ptr, str.s, str.l);
            free(str.s);
            int p_free = fmt->p_free;
            bcf_unpack_fmt_core1(ptr, line->n_sample, fmt);
            fmt->p_free = p_free;
        }
        else
        {
            if ( fmt->p_free )
                free(fmt->p - fmt->p_off);
            bcf_unpack_fmt_core1((uint8_t*)str.s, line->n_sample, fmt);
            fmt->p_free = 1;
            line->d.indiv_dirty = 1;
        }
    }
    line->unpacked |= BCF_UN_FMT;
    return 0;
}