int bcf_translate(const bcf_hdr_t *dst_hdr, bcf_hdr_t *src_hdr, bcf1_t *line)
{
    int i;
    if ( line->errcode )
    {
        hts_log_error("Unchecked error (%d) at %s:%"PRIhts_pos", exiting", line->errcode, bcf_seqname_safe(src_hdr,line), line->pos+1);
        exit(1);
    }
    if ( src_hdr->ntransl==-1 ) return 0;    // no need to translate, all tags have the same id
    if ( !src_hdr->ntransl )  // called for the first time, see what needs translating
    {
        int dict;
        for (dict=0; dict<2; dict++)    // BCF_DT_ID and BCF_DT_CTG
        {
            src_hdr->transl[dict] = (int*) malloc(src_hdr->n[dict]*sizeof(int));
            for (i=0; i<src_hdr->n[dict]; i++)
            {
                if ( !src_hdr->id[dict][i].key ) // gap left after removed BCF header lines
                {
                    src_hdr->transl[dict][i] = -1;
                    continue;
                }
                src_hdr->transl[dict][i] = bcf_hdr_id2int(dst_hdr,dict,src_hdr->id[dict][i].key);
                if ( src_hdr->transl[dict][i]!=-1 && i!=src_hdr->transl[dict][i] ) src_hdr->ntransl++;
            }
        }
        if ( !src_hdr->ntransl )
        {
            free(src_hdr->transl[0]); src_hdr->transl[0] = NULL;
            free(src_hdr->transl[1]); src_hdr->transl[1] = NULL;
            src_hdr->ntransl = -1;
        }
        if ( src_hdr->ntransl==-1 ) return 0;
    }
    bcf_unpack(line,BCF_UN_ALL);

    // CHROM
    if ( src_hdr->transl[BCF_DT_CTG][line->rid] >=0 ) line->rid = src_hdr->transl[BCF_DT_CTG][line->rid];

    // FILTER
    for (i=0; i<line->d.n_flt; i++)
    {
        int src_id = line->d.flt[i];
        if ( src_hdr->transl[BCF_DT_ID][src_id] >=0 )
            line->d.flt[i] = src_hdr->transl[BCF_DT_ID][src_id];
        line->d.shared_dirty |= BCF1_DIRTY_FLT;
    }

    // INFO
    for (i=0; i<line->n_info; i++)
    {
        int src_id = line->d.info[i].key;
        int dst_id = src_hdr->transl[BCF_DT_ID][src_id];
        if ( dst_id<0 ) continue;
        line->d.info[i].key = dst_id;
        if ( !line->d.info[i].vptr ) continue;  // skip deleted
        int src_size = src_id>>7 ? ( src_id>>15 ? BCF_BT_INT32 : BCF_BT_INT16) : BCF_BT_INT8;
        int dst_size = dst_id>>7 ? ( dst_id>>15 ? BCF_BT_INT32 : BCF_BT_INT16) : BCF_BT_INT8;
        if ( src_size==dst_size )   // can overwrite
        {
            uint8_t *vptr = line->d.info[i].vptr - line->d.info[i].vptr_off;
            if ( dst_size==BCF_BT_INT8 ) { vptr[1] = (uint8_t)dst_id; }
            else if ( dst_size==BCF_BT_INT16 ) { *(uint16_t*)vptr = (uint16_t)dst_id; }
            else { *(uint32_t*)vptr = (uint32_t)dst_id; }
        }
        else    // must realloc
        {
            bcf_info_t *info = &line->d.info[i];
            kstring_t str = {0,0,0};
            bcf_enc_int1(&str, dst_id);
            bcf_enc_size(&str, info->len,info->type);
            uint32_t vptr_off = str.l;
            kputsn((char*)info->vptr, info->vptr_len, &str);
            if( info->vptr_free ) free(info->vptr - info->vptr_off);
            info->vptr_off = vptr_off;
            info->vptr = (uint8_t*)str.s + info->vptr_off;
            info->vptr_free = 1;
            line->d.shared_dirty |= BCF1_DIRTY_INF;
        }
    }

    // FORMAT
    for (i=0; i<line->n_fmt; i++)
    {
        int src_id = line->d.fmt[i].id;
        int dst_id = src_hdr->transl[BCF_DT_ID][src_id];
        if ( dst_id<0 ) continue;
        line->d.fmt[i].id = dst_id;
        if( !line->d.fmt[i].p ) continue;  // skip deleted
        int src_size = src_id>>7 ? ( src_id>>15 ? BCF_BT_INT32 : BCF_BT_INT16) : BCF_BT_INT8;
        int dst_size = dst_id>>7 ? ( dst_id>>15 ? BCF_BT_INT32 : BCF_BT_INT16) : BCF_BT_INT8;
        if ( src_size==dst_size )   // can overwrite
        {
            uint8_t *p = line->d.fmt[i].p - line->d.fmt[i].p_off;    // pointer to the vector size (4bits) and BT type (4bits)
            if ( dst_size==BCF_BT_INT8 ) { p[1] = dst_id; }
            else if ( dst_size==BCF_BT_INT16 ) { i16_to_le(dst_id, p + 1); }
            else { i32_to_le(dst_id, p + 1); }
        }
        else    // must realloc
        {
            bcf_fmt_t *fmt = &line->d.fmt[i];
            kstring_t str = {0,0,0};
            bcf_enc_int1(&str, dst_id);
            bcf_enc_size(&str, fmt->n, fmt->type);
            uint32_t p_off = str.l;
            kputsn((char*)fmt->p, fmt->p_len, &str);
            if( fmt->p_free ) free(fmt->p - fmt->p_off);
            fmt->p_off = p_off;
            fmt->p = (uint8_t*)str.s + fmt->p_off;
            fmt->p_free = 1;
            line->d.indiv_dirty = 1;
        }
    }
    return 0;
}