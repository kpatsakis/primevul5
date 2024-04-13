int vcf_write(htsFile *fp, const bcf_hdr_t *h, bcf1_t *v)
{
    int ret;
    fp->line.l = 0;
    if (vcf_format1(h, v, &fp->line) != 0)
        return -1;
    if ( fp->format.compression!=no_compression )
        ret = bgzf_write(fp->fp.bgzf, fp->line.s, fp->line.l);
    else
        ret = hwrite(fp->fp.hfile, fp->line.s, fp->line.l);

    if (fp->idx) {
        int tid;
        if ((tid = hts_idx_tbi_name(fp->idx, v->rid, bcf_seqname_safe(h, v))) < 0)
            return -1;

        if (hts_idx_push(fp->idx, tid, v->pos, v->pos + v->rlen, bgzf_tell(fp->fp.bgzf), 1) < 0)
            return -1;
    }

    return ret==fp->line.l ? 0 : -1;
}