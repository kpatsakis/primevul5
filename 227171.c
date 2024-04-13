static int bcf_dec_typed_int1_safe(uint8_t *p, uint8_t *end, uint8_t **q,
                                   int32_t *val) {
    uint32_t t;
    if (end - p < 2) return -1;
    t = *p++ & 0xf;
    /* Use if .. else if ... else instead of switch to force order.  Assumption
       is that small integers are more frequent than big ones. */
    if (t == BCF_BT_INT8) {
        *q = p + 1;
        *val = *(int8_t *) p;
    } else if (t == BCF_BT_INT16) {
        if (end - p < 2) return -1;
        *q = p + 2;
        *val = le_to_i16(p);
    } else if (t == BCF_BT_INT32) {
        if (end - p < 4) return -1;
        *q = p + 4;
        *val = le_to_i32(p);
#ifdef VCF_ALLOW_INT64
    } else if (t == BCF_BT_INT64) {
        // This case should never happen because there should be no 64-bit BCFs
        // at all, definitely not coming from htslib
        if (end - p < 8) return -1;
        *q = p + 8;
        *val = le_to_i64(p);
#endif
    } else {
        return -1;
    }
    return 0;
}