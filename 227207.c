static inline int align_mem(kstring_t *s)
{
    int e = 0;
    if (s->l&7) {
        uint64_t zero = 0;
        e = kputsn((char*)&zero, 8 - (s->l&7), s) < 0;
    }
    return e == 0 ? 0 : -1;
}