int vcf_format(const bcf_hdr_t *h, const bcf1_t *v, kstring_t *s)
{
    int i;
    int32_t max_dt_id = h->n[BCF_DT_ID];
    const char *chrom = bcf_seqname(h, v);
    if (!chrom) {
        hts_log_error("Invalid BCF, CONTIG id=%d not present in the header",
                      v->rid);
        errno = EINVAL;
        return -1;
    }
    bcf_unpack((bcf1_t*)v, BCF_UN_ALL);
    kputs(chrom, s); // CHROM
    kputc('\t', s); kputll(v->pos + 1, s); // POS
    kputc('\t', s); kputs(v->d.id ? v->d.id : ".", s); // ID
    kputc('\t', s); // REF
    if (v->n_allele > 0) kputs(v->d.allele[0], s);
    else kputc('.', s);
    kputc('\t', s); // ALT
    if (v->n_allele > 1) {
        for (i = 1; i < v->n_allele; ++i) {
            if (i > 1) kputc(',', s);
            kputs(v->d.allele[i], s);
        }
    } else kputc('.', s);
    kputc('\t', s); // QUAL
    if ( bcf_float_is_missing(v->qual) ) kputc('.', s); // QUAL
    else kputd(v->qual, s);
    kputc('\t', s); // FILTER
    if (v->d.n_flt) {
        for (i = 0; i < v->d.n_flt; ++i) {
            int32_t idx = v->d.flt[i];
            if (idx < 0 || idx >= max_dt_id
                || h->id[BCF_DT_ID][idx].key == NULL) {
                hts_log_error("Invalid BCF, the FILTER tag id=%d at %s:%"PRIhts_pos" not present in the header",
                              idx, bcf_seqname_safe(h, v), v->pos + 1);
                errno = EINVAL;
                return -1;
            }
            if (i) kputc(';', s);
            kputs(h->id[BCF_DT_ID][idx].key, s);
        }
    } else kputc('.', s);
    kputc('\t', s); // INFO
    if (v->n_info) {
        int first = 1;
        for (i = 0; i < v->n_info; ++i) {
            bcf_info_t *z = &v->d.info[i];
            if ( !z->vptr ) continue;
            if ( !first ) kputc(';', s);
            first = 0;
            if (z->key < 0 || z->key >= max_dt_id
                || h->id[BCF_DT_ID][z->key].key == NULL) {
                hts_log_error("Invalid BCF, the INFO tag id=%d is %s at %s:%"PRIhts_pos,
                              z->key,
                              z->key < 0 ? "negative"
                              : (z->key >= max_dt_id ? "too large" : "not present in the header"),
                              bcf_seqname_safe(h, v), v->pos+1);
                errno = EINVAL;
                return -1;
            }
            kputs(h->id[BCF_DT_ID][z->key].key, s);
            if (z->len <= 0) continue;
            kputc('=', s);
            if (z->len == 1)
            {
                switch (z->type)
                {
                    case BCF_BT_INT8:  if ( z->v1.i==bcf_int8_missing ) kputc('.', s); else kputw(z->v1.i, s); break;
                    case BCF_BT_INT16: if ( z->v1.i==bcf_int16_missing ) kputc('.', s); else kputw(z->v1.i, s); break;
                    case BCF_BT_INT32: if ( z->v1.i==bcf_int32_missing ) kputc('.', s); else kputw(z->v1.i, s); break;
                    case BCF_BT_INT64: if ( z->v1.i==bcf_int64_missing ) kputc('.', s); else kputll(z->v1.i, s); break;
                    case BCF_BT_FLOAT: if ( bcf_float_is_missing(z->v1.f) ) kputc('.', s); else kputd(z->v1.f, s); break;
                    case BCF_BT_CHAR:  kputc(z->v1.i, s); break;
                    default:
                        hts_log_error("Unexpected type %d at %s:%"PRIhts_pos, z->type, bcf_seqname_safe(h, v), v->pos+1);
                        errno = EINVAL;
                        return -1;
                }
            }
            else bcf_fmt_array(s, z->len, z->type, z->vptr);
        }
        if ( first ) kputc('.', s);
    } else kputc('.', s);
    // FORMAT and individual information
    if (v->n_sample)
    {
        int i,j;
        if ( v->n_fmt)
        {
            int gt_i = -1;
            bcf_fmt_t *fmt = v->d.fmt;
            int first = 1;
            for (i = 0; i < (int)v->n_fmt; ++i) {
                if ( !fmt[i].p ) continue;
                kputc(!first ? ':' : '\t', s); first = 0;
                if (fmt[i].id < 0 || fmt[i].id >= max_dt_id
                    || h->id[BCF_DT_ID][fmt[i].id].key == NULL) //!bcf_hdr_idinfo_exists(h,BCF_HL_FMT,fmt[i].id) )
                {
                    hts_log_error("Invalid BCF, the FORMAT tag id=%d at %s:%"PRIhts_pos" not present in the header", fmt[i].id, bcf_seqname_safe(h, v), v->pos+1);
                    errno = EINVAL;
                    return -1;
                }
                kputs(h->id[BCF_DT_ID][fmt[i].id].key, s);
                if (strcmp(h->id[BCF_DT_ID][fmt[i].id].key, "GT") == 0) gt_i = i;
            }
            if ( first ) kputs("\t.", s);
            for (j = 0; j < v->n_sample; ++j) {
                kputc('\t', s);
                first = 1;
                for (i = 0; i < (int)v->n_fmt; ++i) {
                    bcf_fmt_t *f = &fmt[i];
                    if ( !f->p ) continue;
                    if (!first) kputc(':', s);
                    first = 0;
                    if (gt_i == i)
                        bcf_format_gt(f,j,s);
                    else
                        bcf_fmt_array(s, f->n, f->type, f->p + j * (size_t)f->size);
                }
                if ( first ) kputc('.', s);
            }
        }
        else
            for (j=0; j<=v->n_sample; j++)
                kputs("\t.", s);
    }
    kputc('\n', s);
    return 0;
}