int vcf_read(htsFile *fp, const bcf_hdr_t *h, bcf1_t *v)
{
    int ret;
    ret = hts_getline(fp, KS_SEP_LINE, &fp->line);
    if (ret < 0) return ret;
    return vcf_parse1(&fp->line, h, v);
}