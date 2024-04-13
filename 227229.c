hts_idx_t *bcf_index(htsFile *fp, int min_shift)
{
    int n_lvls;
    bcf1_t *b = NULL;
    hts_idx_t *idx = NULL;
    bcf_hdr_t *h;
    int r;
    h = bcf_hdr_read(fp);
    if ( !h ) return NULL;
    int nids = 0;
    n_lvls = idx_calc_n_lvls_ids(h, min_shift, 0, &nids);
    idx = hts_idx_init(nids, HTS_FMT_CSI, bgzf_tell(fp->fp.bgzf), min_shift, n_lvls);
    if (!idx) goto fail;
    b = bcf_init1();
    if (!b) goto fail;
    while ((r = bcf_read1(fp,h, b)) >= 0) {
        int ret;
        ret = hts_idx_push(idx, b->rid, b->pos, b->pos + b->rlen, bgzf_tell(fp->fp.bgzf), 1);
        if (ret < 0) goto fail;
    }
    if (r < -1) goto fail;
    hts_idx_finish(idx, bgzf_tell(fp->fp.bgzf));
    bcf_destroy1(b);
    bcf_hdr_destroy(h);
    return idx;

 fail:
    hts_idx_destroy(idx);
    bcf_destroy1(b);
    bcf_hdr_destroy(h);
    return NULL;
}