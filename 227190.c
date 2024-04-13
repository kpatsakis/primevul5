int bcf_fmt_array(kstring_t *s, int n, int type, void *data)
{
    int j = 0;
    uint32_t e = 0;
    if (n == 0) {
        return kputc('.', s) >= 0 ? 0 : -1;
    }
    if (type == BCF_BT_CHAR)
    {
        char *p = (char*)data;
        for (j = 0; j < n && *p; ++j, ++p)
        {
            if ( *p==bcf_str_missing ) e |= kputc('.', s) < 0;
            else e |= kputc(*p, s) < 0;
        }
    }
    else
    {
        #define BRANCH(type_t, convert, is_missing, is_vector_end, kprint) { \
            uint8_t *p = (uint8_t *) data; \
            for (j=0; j<n; j++, p += sizeof(type_t))    \
            { \
                type_t v = convert(p); \
                if ( is_vector_end ) break; \
                if ( j ) kputc(',', s); \
                if ( is_missing ) kputc('.', s); \
                else e |= kprint < 0; \
            } \
        }
        switch (type) {
            case BCF_BT_INT8:  BRANCH(int8_t,  le_to_i8, v==bcf_int8_missing,  v==bcf_int8_vector_end,  kputw(v, s)); break;
            case BCF_BT_INT16: BRANCH(int16_t, le_to_i16, v==bcf_int16_missing, v==bcf_int16_vector_end, kputw(v, s)); break;
            case BCF_BT_INT32: BRANCH(int32_t, le_to_i32, v==bcf_int32_missing, v==bcf_int32_vector_end, kputw(v, s)); break;
            case BCF_BT_FLOAT: BRANCH(uint32_t, le_to_u32, v==bcf_float_missing, v==bcf_float_vector_end, kputd(le_to_float(p), s)); break;
            default: hts_log_error("Unexpected type %d", type); exit(1); break;
        }
        #undef BRANCH
    }
    return e == 0 ? 0 : -1;
}