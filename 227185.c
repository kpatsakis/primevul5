int bcf_subset_format(const bcf_hdr_t *hdr, bcf1_t *rec)
{
    if ( !hdr->keep_samples ) return 0;
    if ( !bcf_hdr_nsamples(hdr) )
    {
        rec->indiv.l = rec->n_sample = 0;
        return 0;
    }

    int i, j;
    uint8_t *ptr = (uint8_t*)rec->indiv.s, *dst = NULL, *src;
    bcf_dec_t *dec = &rec->d;
    hts_expand(bcf_fmt_t, rec->n_fmt, dec->m_fmt, dec->fmt);
    for (i=0; i<dec->m_fmt; ++i) dec->fmt[i].p_free = 0;

    for (i=0; i<rec->n_fmt; i++)
    {
        ptr = bcf_unpack_fmt_core1(ptr, rec->n_sample, &dec->fmt[i]);
        src = dec->fmt[i].p - dec->fmt[i].size;
        if ( dst )
        {
            memmove(dec->fmt[i-1].p + dec->fmt[i-1].p_len, dec->fmt[i].p - dec->fmt[i].p_off, dec->fmt[i].p_off);
            dec->fmt[i].p = dec->fmt[i-1].p + dec->fmt[i-1].p_len + dec->fmt[i].p_off;
        }
        dst = dec->fmt[i].p;
        for (j=0; j<hdr->nsamples_ori; j++)
        {
            src += dec->fmt[i].size;
            if ( !bit_array_test(hdr->keep_samples,j) ) continue;
            memmove(dst, src, dec->fmt[i].size);
            dst += dec->fmt[i].size;
        }
        rec->indiv.l -= dec->fmt[i].p_len - (dst - dec->fmt[i].p);
        dec->fmt[i].p_len = dst - dec->fmt[i].p;
    }
    rec->unpacked |= BCF_UN_FMT;

    rec->n_sample = bcf_hdr_nsamples(hdr);
    return 0;
}