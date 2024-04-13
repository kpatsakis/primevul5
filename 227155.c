int bcf_read(htsFile *fp, const bcf_hdr_t *h, bcf1_t *v)
{
    if (fp->format.format == vcf) return vcf_read(fp,h,v);
    int ret = bcf_read1_core(fp->fp.bgzf, v);
    if (ret == 0) ret = bcf_record_check(h, v);
    if ( ret!=0 || !h->keep_samples ) return ret;
    return bcf_subset_format(h,v);
}