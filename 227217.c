static inline int serialize_float_array(kstring_t *s, size_t n, const float *a) {
    uint8_t *p;
    size_t i;
    size_t bytes = n * sizeof(float);

    if (bytes / sizeof(float) != n) return -1;
    if (ks_resize(s, s->l + bytes) < 0) return -1;

    p = (uint8_t *) s->s + s->l;
    for (i = 0; i < n; i++) {
        float_to_le(a[i], p);
        p += sizeof(float);
    }
    s->l += bytes;

    return 0;
}