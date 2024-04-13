rb_enc_nth(const char *p, const char *e, long nth, rb_encoding *enc)
{
    if (rb_enc_mbmaxlen(enc) == 1) {
        p += nth;
    }
    else if (rb_enc_mbmaxlen(enc) == rb_enc_mbminlen(enc)) {
        p += nth * rb_enc_mbmaxlen(enc);
    }
    else if (rb_enc_asciicompat(enc)) {
        const char *p2, *e2;
        int n;

        while (p < e && 0 < nth) {
            e2 = p + nth;
            if (e < e2)
                return (char *)e;
            if (ISASCII(*p)) {
                p2 = search_nonascii(p, e2);
                if (!p2)
                    return (char *)e2;
                nth -= p2 - p;
                p = p2;
            }
            n = rb_enc_mbclen(p, e, enc);
            p += n;
            nth--;
        }
        if (nth != 0)
            return (char *)e;
        return (char *)p;
    }
    else {
        while (p<e && nth--) {
            p += rb_enc_mbclen(p, e, enc);
        }
    }
    if (p > e) p = e;
    return (char*)p;
}