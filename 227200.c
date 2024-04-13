static int vcf_parse_info(kstring_t *str, const bcf_hdr_t *h, bcf1_t *v, char *p, char *q) {
    static int extreme_int_warned = 0, negative_rlen_warned = 0;
    int max_n_val = 0, overflow = 0;
    char *r, *key;
    khint_t k;
    vdict_t *d = (vdict_t*)h->dict[BCF_DT_ID];
    int32_t *a_val = NULL;

    v->n_info = 0;
    if (*(q-1) == ';') *(q-1) = 0;
    for (r = key = p;; ++r) {
        int c;
        char *val, *end;
        if (*r != ';' && *r != '=' && *r != 0) continue;
        if (v->n_info == UINT16_MAX) {
            hts_log_error("Too many INFO entries at %s:%"PRIhts_pos,
                          bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_LIMITS;
            return -1;
        }
        val = end = 0;
        c = *r; *r = 0;
        if (c == '=') {
            val = r + 1;
            for (end = val; *end != ';' && *end != 0; ++end);
            c = *end; *end = 0;
        } else end = r;
        if ( !*key ) { if (c==0) break; r = end; key = r + 1; continue; }  // faulty VCF, ";;" in the INFO
        k = kh_get(vdict, d, key);
        if (k == kh_end(d) || kh_val(d, k).info[BCF_HL_INFO] == 15)
        {
            hts_log_warning("INFO '%s' is not defined in the header, assuming Type=String", key);
            kstring_t tmp = {0,0,0};
            int l;
            ksprintf(&tmp, "##INFO=<ID=%s,Number=1,Type=String,Description=\"Dummy\">", key);
            bcf_hrec_t *hrec = bcf_hdr_parse_line(h,tmp.s,&l);
            free(tmp.s);
            int res = hrec ? bcf_hdr_add_hrec((bcf_hdr_t*)h, hrec) : -1;
            if (res < 0) bcf_hrec_destroy(hrec);
            if (res > 0) res = bcf_hdr_sync((bcf_hdr_t*)h);
            k = kh_get(vdict, d, key);
            v->errcode = BCF_ERR_TAG_UNDEF;
            if (res || k == kh_end(d)) {
                hts_log_error("Could not add dummy header for INFO '%s' at %s:%"PRIhts_pos, key, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_TAG_INVALID;
                return -1;
            }
        }
        uint32_t y = kh_val(d, k).info[BCF_HL_INFO];
        ++v->n_info;
        bcf_enc_int1(str, kh_val(d, k).id);
        if (val == 0) {
            bcf_enc_size(str, 0, BCF_BT_NULL);
        } else if ((y>>4&0xf) == BCF_HT_FLAG || (y>>4&0xf) == BCF_HT_STR) { // if Flag has a value, treat it as a string
            bcf_enc_vchar(str, end - val, val);
        } else { // int/float value/array
            int i, n_val;
            char *t, *te;
            for (t = val, n_val = 1; *t; ++t) // count the number of values
                if (*t == ',') ++n_val;
            // Check both int and float size in one step for simplicity
            if (n_val > max_n_val) {
                int32_t *a_tmp = (int32_t *)realloc(a_val, n_val * sizeof(*a_val));
                if (!a_tmp) {
                    hts_log_error("Could not allocate memory at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
                    v->errcode |= BCF_ERR_LIMITS; // No appropriate code?
                    return -1;
                }
                a_val = a_tmp;
                max_n_val = n_val;
            }
            if ((y>>4&0xf) == BCF_HT_INT) {
                i = 0, t = val;
                int64_t val1;
                int is_int64 = 0;
#ifdef VCF_ALLOW_INT64
                if ( n_val==1 )
                {
                    overflow = 0;
                    long long int tmp_val = hts_str2int(val, &te, sizeof(tmp_val)*CHAR_BIT, &overflow);
                    if ( te==val ) tmp_val = bcf_int32_missing;
                    else if ( overflow || tmp_val<BCF_MIN_BT_INT64 || tmp_val>BCF_MAX_BT_INT64 )
                    {
                        if ( !extreme_int_warned )
                        {
                            hts_log_warning("Extreme INFO/%s value encountered and set to missing at %s:%"PRIhts_pos,key,bcf_seqname_safe(h,v), v->pos+1);
                            extreme_int_warned = 1;
                        }
                        tmp_val = bcf_int32_missing;
                    }
                    else
                        is_int64 = 1;
                    val1 = tmp_val;
                    t = te;
                    i = 1;  // this is just to avoid adding another nested block...
                }
#endif
                for (; i < n_val; ++i, ++t)
                {
                    overflow = 0;
                    long int tmp_val = hts_str2int(t, &te, sizeof(tmp_val)*CHAR_BIT, &overflow);
                    if ( te==t ) tmp_val = bcf_int32_missing;
                    else if ( overflow || tmp_val<BCF_MIN_BT_INT32 || tmp_val>BCF_MAX_BT_INT32 )
                    {
                        if ( !extreme_int_warned )
                        {
                            hts_log_warning("Extreme INFO/%s value encountered and set to missing at %s:%"PRIhts_pos,key,bcf_seqname_safe(h,v), v->pos+1);
                            extreme_int_warned = 1;
                        }
                        tmp_val = bcf_int32_missing;
                    }
                    a_val[i] = tmp_val;
                    for (t = te; *t && *t != ','; t++);
                }
                if (n_val == 1) {
#ifdef VCF_ALLOW_INT64
                    if ( is_int64 )
                    {
                        v->unpacked |= BCF_IS_64BIT;
                        bcf_enc_long1(str, val1);
                    }
                    else
                        bcf_enc_int1(str, (int32_t)val1);
#else
                    val1 = a_val[0];
                    bcf_enc_int1(str, (int32_t)val1);
#endif
                } else {
                    bcf_enc_vint(str, n_val, a_val, -1);
                }
                if (n_val==1 && (val1!=bcf_int32_missing || is_int64) && strcmp(key, "END") == 0)
                {
                    if ( val1 <= v->pos )
                    {
                        if ( !negative_rlen_warned )
                        {
                            hts_log_warning("INFO/END=%"PRIhts_pos" is smaller than POS at %s:%"PRIhts_pos,val1,bcf_seqname_safe(h,v),v->pos+1);
                            negative_rlen_warned = 1;
                        }
                    }
                    else
                        v->rlen = val1 - v->pos;
                }
            } else if ((y>>4&0xf) == BCF_HT_REAL) {
                float *val_f = (float *)a_val;
                for (i = 0, t = val; i < n_val; ++i, ++t)
                {
                    overflow = 0;
                    val_f[i] = hts_str2dbl(t, &te, &overflow);
                    if ( te==t || overflow ) // conversion failed
                        bcf_float_set_missing(val_f[i]);
                    for (t = te; *t && *t != ','; t++);
                }
                bcf_enc_vfloat(str, n_val, val_f);
            }
        }
        if (c == 0) break;
        r = end;
        key = r + 1;
    }

    free(a_val);
    return 0;
}