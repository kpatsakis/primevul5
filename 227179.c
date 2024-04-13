int bcf_write(htsFile *hfp, bcf_hdr_t *h, bcf1_t *v)
{
    if ( h->dirty ) {
        if (bcf_hdr_sync(h) < 0) return -1;
    }
    if ( bcf_hdr_nsamples(h)!=v->n_sample )
    {
        hts_log_error("Broken VCF record, the number of columns at %s:%"PRIhts_pos" does not match the number of samples (%d vs %d)",
            bcf_seqname_safe(h,v), v->pos+1, v->n_sample, bcf_hdr_nsamples(h));
        return -1;
    }

    if ( hfp->format.format == vcf || hfp->format.format == text_format )
        return vcf_write(hfp,h,v);

    if ( v->errcode )
    {
        // vcf_parse1() encountered a new contig or tag, undeclared in the
        // header.  At this point, the header must have been printed,
        // proceeding would lead to a broken BCF file. Errors must be checked
        // and cleared by the caller before we can proceed.
        hts_log_error("Unchecked error (%d) at %s:%"PRIhts_pos, v->errcode, bcf_seqname_safe(h,v), v->pos+1);
        return -1;
    }
    bcf1_sync(v);   // check if the BCF record was modified

    if ( v->unpacked & BCF_IS_64BIT )
    {
        hts_log_error("Data at %s:%"PRIhts_pos" contains 64-bit values not representable in BCF. Please use VCF instead", bcf_seqname_safe(h,v), v->pos+1);
        return -1;
    }

    BGZF *fp = hfp->fp.bgzf;
    uint8_t x[32];
    u32_to_le(v->shared.l + 24, x); // to include six 32-bit integers
    u32_to_le(v->indiv.l, x + 4);
    i32_to_le(v->rid, x + 8);
    u32_to_le(v->pos, x + 12);
    u32_to_le(v->rlen, x + 16);
    float_to_le(v->qual, x + 20);
    u16_to_le(v->n_info, x + 24);
    u16_to_le(v->n_allele, x + 26);
    u32_to_le((uint32_t)v->n_fmt<<24 | (v->n_sample & 0xffffff), x + 28);
    if ( bgzf_write(fp, x, 32) != 32 ) return -1;
    if ( bgzf_write(fp, v->shared.s, v->shared.l) != v->shared.l ) return -1;
    if ( bgzf_write(fp, v->indiv.s, v->indiv.l) != v->indiv.l ) return -1;

    if (hfp->idx) {
        if (hts_idx_push(hfp->idx, v->rid, v->pos, v->pos + v->rlen, bgzf_tell(fp), 1) < 0)
            return -1;
    }

    return 0;
}