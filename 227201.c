int bcf_get_info_values(const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, void **dst, int *ndst, int type)
{
    int i, ret = -4, tag_id = bcf_hdr_id2int(hdr, BCF_DT_ID, tag);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_INFO,tag_id) ) return -1;    // no such INFO field in the header
    if ( bcf_hdr_id2type(hdr,BCF_HL_INFO,tag_id)!=(type & 0xff) ) return -2;     // expected different type

    if ( !(line->unpacked & BCF_UN_INFO) ) bcf_unpack(line, BCF_UN_INFO);

    for (i=0; i<line->n_info; i++)
        if ( line->d.info[i].key==tag_id ) break;
    if ( i==line->n_info ) return ( type==BCF_HT_FLAG ) ? 0 : -3;       // the tag is not present in this record
    if ( type==BCF_HT_FLAG ) return 1;

    bcf_info_t *info = &line->d.info[i];
    if ( !info->vptr ) return -3;           // the tag was marked for removal
    if ( type==BCF_HT_STR )
    {
        if ( *ndst < info->len+1 )
        {
            *ndst = info->len + 1;
            *dst  = realloc(*dst, *ndst);
        }
        memcpy(*dst,info->vptr,info->len);
        ((uint8_t*)*dst)[info->len] = 0;
        return info->len;
    }

    // Make sure the buffer is big enough
    int size1;
    switch (type) {
        case BCF_HT_INT:  size1 = sizeof(int32_t); break;
        case BCF_HT_LONG: size1 = sizeof(int64_t); break;
        case BCF_HT_REAL: size1 = sizeof(float); break;
        default:
            hts_log_error("Unexpected output type %d at %s:%"PRIhts_pos, type, bcf_seqname_safe(hdr,line), line->pos+1);
            return -2;
    }
    if ( *ndst < info->len )
    {
        *ndst = info->len;
        *dst  = realloc(*dst, *ndst * size1);
    }

    #define BRANCH(type_t, convert, is_missing, is_vector_end, set_missing, set_regular, out_type_t) do { \
        out_type_t *tmp = (out_type_t *) *dst; \
        int j; \
        for (j=0; j<info->len; j++) \
        { \
            type_t p = convert(info->vptr + j * sizeof(type_t)); \
            if ( is_vector_end ) break; \
            if ( is_missing ) set_missing; \
            else set_regular; \
            tmp++; \
        } \
        ret = j; \
    } while (0)
    switch (info->type) {
        case BCF_BT_INT8:
            if (type == BCF_HT_LONG) {
                BRANCH(int8_t,  le_to_i8,  p==bcf_int8_missing,  p==bcf_int8_vector_end,  *tmp=bcf_int64_missing, *tmp=p, int64_t);
            } else {
                BRANCH(int8_t,  le_to_i8,  p==bcf_int8_missing,  p==bcf_int8_vector_end,  *tmp=bcf_int32_missing, *tmp=p, int32_t);
            }
            break;
        case BCF_BT_INT16:
            if (type == BCF_HT_LONG) {
                BRANCH(int16_t, le_to_i16, p==bcf_int16_missing, p==bcf_int16_vector_end, *tmp=bcf_int64_missing, *tmp=p, int64_t);
            } else {
                BRANCH(int16_t, le_to_i16, p==bcf_int16_missing, p==bcf_int16_vector_end, *tmp=bcf_int32_missing, *tmp=p, int32_t);
            }
            break;
        case BCF_BT_INT32:
            if (type == BCF_HT_LONG) {
                BRANCH(int32_t, le_to_i32, p==bcf_int32_missing, p==bcf_int32_vector_end, *tmp=bcf_int64_missing, *tmp=p, int64_t); break;
            } else {
                BRANCH(int32_t, le_to_i32, p==bcf_int32_missing, p==bcf_int32_vector_end, *tmp=bcf_int32_missing, *tmp=p, int32_t); break;
            }
        case BCF_BT_FLOAT: BRANCH(uint32_t, le_to_u32, p==bcf_float_missing, p==bcf_float_vector_end, bcf_float_set_missing(*tmp), bcf_float_set(tmp, p), float); break;
        default: hts_log_error("Unexpected type %d at %s:%"PRIhts_pos, info->type, bcf_seqname_safe(hdr,line), line->pos+1); return -2;
    }
    #undef BRANCH
    return ret;  // set by BRANCH
}