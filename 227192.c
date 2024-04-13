static int bcf_dec_size_safe(uint8_t *p, uint8_t *end, uint8_t **q,
                             int *num, int *type) {
    int r;
    if (p >= end) return -1;
    *type = *p & 0xf;
    if (*p>>4 != 15) {
        *q = p + 1;
        *num = *p >> 4;
        return 0;
    }
    r = bcf_dec_typed_int1_safe(p + 1, end, q, num);
    if (r) return r;
    return *num >= 0 ? 0 : -1;
}