search_nonascii(const char *p, const char *e)
{
#if SIZEOF_VALUE == 8
# define NONASCII_MASK 0x8080808080808080LL
#elif SIZEOF_VALUE == 4
# define NONASCII_MASK 0x80808080UL
#endif
#ifdef NONASCII_MASK
    if ((int)sizeof(VALUE) * 2 < e - p) {
        const VALUE *s, *t;
        const VALUE lowbits = sizeof(VALUE) - 1;
        s = (const VALUE*)(~lowbits & ((VALUE)p + lowbits));
        while (p < (const char *)s) {
            if (!ISASCII(*p))
                return p;
            p++;
        }
        t = (const VALUE*)(~lowbits & (VALUE)e);
        while (s < t) {
            if (*s & NONASCII_MASK) {
                t = s;
                break;
            }
            s++;
        }
        p = (const char *)t;
    }
#endif
    while (p < e) {
        if (!ISASCII(*p))
            return p;
        p++;
    }
    return NULL;
}