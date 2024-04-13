int bcf_get_format_values(const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, void **dst, int *ndst, int type)
{
    int i,j, tag_id = bcf_hdr_id2int(hdr, BCF_DT_ID, tag);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_FMT,tag_id) ) return -1;    // no such FORMAT field in the header
    if ( tag[0]=='G' && tag[1]=='T' && tag[2]==0 )
    {
        // Ugly: GT field is considered to be a string by the VCF header but BCF represents it as INT.
        if ( bcf_hdr_id2type(hdr,BCF_HL_FMT,tag_id)!=BCF_HT_STR ) return -2;
    }
    else if ( bcf_hdr_id2type(hdr,BCF_HL_FMT,tag_id)!=type ) return -2;     // expected different type

    if ( !(line->unpacked & BCF_UN_FMT) ) bcf_unpack(line, BCF_UN_FMT);

    for (i=0; i<line->n_fmt; i++)
        if ( line->d.fmt[i].id==tag_id ) break;
    if ( i==line->n_fmt ) return -3;                               // the tag is not present in this record
    bcf_fmt_t *fmt = &line->d.fmt[i];
    if ( !fmt->p ) return -3;                                      // the tag was marked for removal

    if ( type==BCF_HT_STR )
    {
        int n = fmt->n*bcf_hdr_nsamples(hdr);
        if ( *ndst < n )
        {
            *dst  = realloc(*dst, n);
            if ( !*dst ) return -4;     // could not alloc
            *ndst = n;
        }
        memcpy(*dst,fmt->p,n);
        return n;
    }

    // Make sure the buffer is big enough
    int nsmpl = bcf_hdr_nsamples(hdr);
    int size1 = type==BCF_HT_INT ? sizeof(int32_t) : sizeof(float);
    if ( *ndst < fmt->n*nsmpl )
    {
        *ndst = fmt->n*nsmpl;
        *dst  = realloc(*dst, *ndst*size1);
        if ( !*dst ) return -4;     // could not alloc
    }

    #define BRANCH(type_t, convert, is_missing, is_vector_end, set_missing, set_vector_end, set_regular, out_type_t) { \
        out_type_t *tmp = (out_type_t *) *dst; \
        uint8_t *fmt_p = fmt->p; \
        for (i=0; i<nsmpl; i++) \
        { \
            for (j=0; j<fmt->n; j++) \
            { \
                type_t p = convert(fmt_p + j * sizeof(type_t)); \
                if ( is_missing ) set_missing; \
                else if ( is_vector_end ) { set_vector_end; break; } \
                else set_regular; \
                tmp++; \
            } \
            for (; j<fmt->n; j++) { set_vector_end; tmp++; } \
            fmt_p += fmt->size; \
        } \
    }
    switch (fmt->type) {
        case BCF_BT_INT8:  BRANCH(int8_t,  le_to_i8, p==bcf_int8_missing,  p==bcf_int8_vector_end,  *tmp=bcf_int32_missing, *tmp=bcf_int32_vector_end, *tmp=p, int32_t); break;
        case BCF_BT_INT16: BRANCH(int16_t, le_to_i16, p==bcf_int16_missing, p==bcf_int16_vector_end, *tmp=bcf_int32_missing, *tmp=bcf_int32_vector_end, *tmp=p, int32_t); break;
        case BCF_BT_INT32: BRANCH(int32_t, le_to_i32, p==bcf_int32_missing, p==bcf_int32_vector_end, *tmp=bcf_int32_missing, *tmp=bcf_int32_vector_end, *tmp=p, int32_t); break;
        case BCF_BT_FLOAT: BRANCH(uint32_t, le_to_u32, p==bcf_float_missing, p==bcf_float_vector_end, bcf_float_set_missing(*tmp), bcf_float_set_vector_end(*tmp), bcf_float_set(tmp, p), float); break;
        default: hts_log_error("Unexpected type %d at %s:%"PRIhts_pos, fmt->type, bcf_seqname_safe(hdr,line), line->pos+1); exit(1);
    }
    #undef BRANCH
    return nsmpl*fmt->n;
}