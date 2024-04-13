int bcf_enc_vint(kstring_t *s, int n, int32_t *a, int wsize)
{
    int32_t max = INT32_MIN, min = INT32_MAX;
    int i;
    if (n <= 0) bcf_enc_size(s, 0, BCF_BT_NULL);
    else if (n == 1) bcf_enc_int1(s, a[0]);
    else {
        if (wsize <= 0) wsize = n;
        for (i = 0; i < n; ++i) {
            if (a[i] == bcf_int32_missing || a[i] == bcf_int32_vector_end ) continue;
            if (max < a[i]) max = a[i];
            if (min > a[i]) min = a[i];
        }
        if (max <= BCF_MAX_BT_INT8 && min >= BCF_MIN_BT_INT8) {
            bcf_enc_size(s, wsize, BCF_BT_INT8);
            for (i = 0; i < n; ++i)
                if ( a[i]==bcf_int32_vector_end ) kputc(bcf_int8_vector_end, s);
                else if ( a[i]==bcf_int32_missing ) kputc(bcf_int8_missing, s);
                else kputc(a[i], s);
        } else if (max <= BCF_MAX_BT_INT16 && min >= BCF_MIN_BT_INT16) {
            uint8_t *p;
            bcf_enc_size(s, wsize, BCF_BT_INT16);
            ks_resize(s, s->l + n * sizeof(int16_t));
            p = (uint8_t *) s->s + s->l;
            for (i = 0; i < n; ++i)
            {
                int16_t x;
                if ( a[i]==bcf_int32_vector_end ) x = bcf_int16_vector_end;
                else if ( a[i]==bcf_int32_missing ) x = bcf_int16_missing;
                else x = a[i];
                i16_to_le(x, p);
                p += sizeof(int16_t);
            }
            s->l += n * sizeof(int16_t);
        } else {
            uint8_t *p;
            bcf_enc_size(s, wsize, BCF_BT_INT32);
            ks_resize(s, s->l + n * sizeof(int32_t));
            p = (uint8_t *) s->s + s->l;
            for (i = 0; i < n; ++i) {
                i32_to_le(a[i], p);
                p += sizeof(int32_t);
            }
            s->l += n * sizeof(int32_t);
        }
    }

    return 0; // FIXME: check for errs in this function
}