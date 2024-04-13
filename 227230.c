int bcf_update_info(const bcf_hdr_t *hdr, bcf1_t *line, const char *key, const void *values, int n, int type)
{
    static int negative_rlen_warned = 0;
    int is_end_tag;

    // Is the field already present?
    int i, inf_id = bcf_hdr_id2int(hdr,BCF_DT_ID,key);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_INFO,inf_id) ) return -1;    // No such INFO field in the header
    if ( !(line->unpacked & BCF_UN_INFO) ) bcf_unpack(line, BCF_UN_INFO);

    is_end_tag = strcmp(key, "END") == 0;

    for (i=0; i<line->n_info; i++)
        if ( inf_id==line->d.info[i].key ) break;
    bcf_info_t *inf = i==line->n_info ? NULL : &line->d.info[i];

    if ( !n || (type==BCF_HT_STR && !values) )
    {
        if ( n==0 && is_end_tag )
            line->rlen = line->n_allele ? strlen(line->d.allele[0]) : 0;
        if ( inf )
        {
            // Mark the tag for removal, free existing memory if necessary
            if ( inf->vptr_free )
            {
                free(inf->vptr - inf->vptr_off);
                inf->vptr_free = 0;
            }
            line->d.shared_dirty |= BCF1_DIRTY_INF;
            inf->vptr = NULL;
            inf->vptr_off = inf->vptr_len = 0;
        }
        return 0;
    }

    if (is_end_tag)
    {
        if (n != 1)
        {
            hts_log_error("END info tag should only have one value at %s:%"PRIhts_pos, bcf_seqname_safe(hdr,line), line->pos+1);
            line->errcode |= BCF_ERR_TAG_INVALID;
            return -1;
        }
        if (type != BCF_HT_INT && type != BCF_HT_LONG)
        {
            hts_log_error("Wrong type (%d) for END info tag at %s:%"PRIhts_pos, type, bcf_seqname_safe(hdr,line), line->pos+1);
            line->errcode |= BCF_ERR_TAG_INVALID;
            return -1;
        }
    }

    // Encode the values and determine the size required to accommodate the values
    kstring_t str = {0,0,0};
    bcf_enc_int1(&str, inf_id);
    if ( type==BCF_HT_INT )
        bcf_enc_vint(&str, n, (int32_t*)values, -1);
    else if ( type==BCF_HT_REAL )
        bcf_enc_vfloat(&str, n, (float*)values);
    else if ( type==BCF_HT_FLAG || type==BCF_HT_STR )
    {
        if ( values==NULL )
            bcf_enc_size(&str, 0, BCF_BT_NULL);
        else
            bcf_enc_vchar(&str, strlen((char*)values), (char*)values);
    }
#ifdef VCF_ALLOW_INT64
    else if ( type==BCF_HT_LONG )
    {
        if (n != 1) {
            hts_log_error("Only storing a single BCF_HT_LONG value is supported at %s:%"PRIhts_pos, bcf_seqname_safe(hdr,line), line->pos+1);
            abort();
        }
        bcf_enc_long1(&str, *(int64_t *) values);
    }
#endif
    else
    {
        hts_log_error("The type %d not implemented yet at %s:%"PRIhts_pos, type, bcf_seqname_safe(hdr,line), line->pos+1);
        abort();
    }

    // Is the INFO tag already present
    if ( inf )
    {
        // Is it big enough to accommodate new block?
        if ( str.l <= inf->vptr_len + inf->vptr_off )
        {
            if ( str.l != inf->vptr_len + inf->vptr_off ) line->d.shared_dirty |= BCF1_DIRTY_INF;
            uint8_t *ptr = inf->vptr - inf->vptr_off;
            memcpy(ptr, str.s, str.l);
            free(str.s);
            int vptr_free = inf->vptr_free;
            bcf_unpack_info_core1(ptr, inf);
            inf->vptr_free = vptr_free;
        }
        else
        {
            if ( inf->vptr_free )
                free(inf->vptr - inf->vptr_off);
            bcf_unpack_info_core1((uint8_t*)str.s, inf);
            inf->vptr_free = 1;
            line->d.shared_dirty |= BCF1_DIRTY_INF;
        }
    }
    else
    {
        // The tag is not present, create new one
        line->n_info++;
        hts_expand0(bcf_info_t, line->n_info, line->d.m_info , line->d.info);
        inf = &line->d.info[line->n_info-1];
        bcf_unpack_info_core1((uint8_t*)str.s, inf);
        inf->vptr_free = 1;
        line->d.shared_dirty |= BCF1_DIRTY_INF;
    }
    line->unpacked |= BCF_UN_INFO;

   if ( n==1 && is_end_tag) {
        hts_pos_t end = type == BCF_HT_INT ? *(int32_t *) values : *(int64_t *) values;
        if ( (type == BCF_HT_INT && end!=bcf_int32_missing) || (type == BCF_HT_LONG && end!=bcf_int64_missing) )
        {
            if ( end <= line->pos )
            {
                if ( !negative_rlen_warned )
                {
                    hts_log_warning("INFO/END=%"PRIhts_pos" is smaller than POS at %s:%"PRIhts_pos,end,bcf_seqname_safe(hdr,line),line->pos+1);
                    negative_rlen_warned = 1;
                }
                line->rlen = line->n_allele ? strlen(line->d.allele[0]) : 0;
            }
            else
                line->rlen = end - line->pos;
        }
    }
    return 0;
}