enc_pred_char(char *p, long len, rb_encoding *enc)
{
    long i;
    int l;
    while (1) {
        for (i = len-1; 0 <= i && (unsigned char)p[i] == 0; i--)
            p[i] = '\xff';
        if (i < 0)
            return NEIGHBOR_WRAPPED;
        --((unsigned char*)p)[i];
        l = rb_enc_precise_mbclen(p, p+len, enc);
        if (MBCLEN_CHARFOUND_P(l)) {
            l = MBCLEN_CHARFOUND_LEN(l);
            if (l == len) {
                return NEIGHBOR_FOUND;
            }
            else {
                memset(p+l, 0, len-l);
            }
        }
        if (MBCLEN_INVALID_P(l) && i < len-1) {
            long len2;
            int l2;
            for (len2 = len-1; 0 < len2; len2--) {
                l2 = rb_enc_precise_mbclen(p, p+len2, enc);
                if (!MBCLEN_INVALID_P(l2))
                    break;
            }
            memset(p+len2+1, 0, len-(len2+1));
        }
    }
}