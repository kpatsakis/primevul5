static int bcf_enc_long1(kstring_t *s, int64_t x) {
    uint32_t e = 0;
    if (x <= BCF_MAX_BT_INT32 && x >= BCF_MIN_BT_INT32)
        return bcf_enc_int1(s, x);
    if (x == bcf_int64_vector_end) {
        e |= bcf_enc_size(s, 1, BCF_BT_INT8);
        e |= kputc(bcf_int8_vector_end, s) < 0;
    } else if (x == bcf_int64_missing) {
        e |= bcf_enc_size(s, 1, BCF_BT_INT8);
        e |= kputc(bcf_int8_missing, s) < 0;
    } else {
        e |= bcf_enc_size(s, 1, BCF_BT_INT64);
        e |= ks_expand(s, 8);
        if (e == 0) { u64_to_le(x, (uint8_t *) s->s + s->l); s->l += 8; }
    }
    return e == 0 ? 0 : -1;
}