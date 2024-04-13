int bcf_unpack(bcf1_t *b, int which)
{
    if ( !b->shared.l ) return 0; // Building a new BCF record from scratch
    uint8_t *ptr = (uint8_t*)b->shared.s, *ptr_ori;
    int i;
    bcf_dec_t *d = &b->d;
    if (which & BCF_UN_FLT) which |= BCF_UN_STR;
    if (which & BCF_UN_INFO) which |= BCF_UN_SHR;
    if ((which&BCF_UN_STR) && !(b->unpacked&BCF_UN_STR))
    {
        kstring_t tmp;

        // ID
        tmp.l = 0; tmp.s = d->id; tmp.m = d->m_id;
        ptr_ori = ptr;
        ptr = bcf_fmt_sized_array(&tmp, ptr);
        b->unpack_size[0] = ptr - ptr_ori;
        kputc('\0', &tmp);
        d->id = tmp.s; d->m_id = tmp.m;

        // REF and ALT are in a single block (d->als) and d->alleles are pointers into this block
        hts_expand(char*, b->n_allele, d->m_allele, d->allele); // NM: hts_expand() is a macro
        tmp.l = 0; tmp.s = d->als; tmp.m = d->m_als;
        ptr_ori = ptr;
        char *o = "";
        for (i = 0; i < b->n_allele; ++i) {
            d->allele[i] = o + tmp.l;
            ptr = bcf_fmt_sized_array(&tmp, ptr);
            kputc('\0', &tmp);
        }
        b->unpack_size[1] = ptr - ptr_ori;
        d->als = tmp.s; d->m_als = tmp.m;

        for (i = 0; i < b->n_allele; ++i)
            d->allele[i] = d->als + (d->allele[i]-o);
        b->unpacked |= BCF_UN_STR;
    }
    if ((which&BCF_UN_FLT) && !(b->unpacked&BCF_UN_FLT)) { // FILTER
        ptr = (uint8_t*)b->shared.s + b->unpack_size[0] + b->unpack_size[1];
        ptr_ori = ptr;
        if (*ptr>>4) {
            int type;
            d->n_flt = bcf_dec_size(ptr, &ptr, &type);
            hts_expand(int, d->n_flt, d->m_flt, d->flt);
            for (i = 0; i < d->n_flt; ++i)
                d->flt[i] = bcf_dec_int1(ptr, type, &ptr);
        } else ++ptr, d->n_flt = 0;
        b->unpack_size[2] = ptr - ptr_ori;
        b->unpacked |= BCF_UN_FLT;
    }
    if ((which&BCF_UN_INFO) && !(b->unpacked&BCF_UN_INFO)) { // INFO
        ptr = (uint8_t*)b->shared.s + b->unpack_size[0] + b->unpack_size[1] + b->unpack_size[2];
        hts_expand(bcf_info_t, b->n_info, d->m_info, d->info);
        for (i = 0; i < d->m_info; ++i) d->info[i].vptr_free = 0;
        for (i = 0; i < b->n_info; ++i)
            ptr = bcf_unpack_info_core1(ptr, &d->info[i]);
        b->unpacked |= BCF_UN_INFO;
    }
    if ((which&BCF_UN_FMT) && b->n_sample && !(b->unpacked&BCF_UN_FMT)) { // FORMAT
        ptr = (uint8_t*)b->indiv.s;
        hts_expand(bcf_fmt_t, b->n_fmt, d->m_fmt, d->fmt);
        for (i = 0; i < d->m_fmt; ++i) d->fmt[i].p_free = 0;
        for (i = 0; i < b->n_fmt; ++i)
            ptr = bcf_unpack_fmt_core1(ptr, b->n_sample, &d->fmt[i]);
        b->unpacked |= BCF_UN_FMT;
    }
    return 0;
}