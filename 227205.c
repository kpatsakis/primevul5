int bcf_idx_init(htsFile *fp, bcf_hdr_t *h, int min_shift, const char *fnidx) {
    int n_lvls, nids = 0;

    if (fp->format.format == vcf)
        return vcf_idx_init(fp, h, min_shift, fnidx);

    if (!min_shift)
        min_shift = 14;

    n_lvls = idx_calc_n_lvls_ids(h, min_shift, 0, &nids);

    fp->idx = hts_idx_init(nids, HTS_FMT_CSI, bgzf_tell(fp->fp.bgzf), min_shift, n_lvls);
    if (!fp->idx) return -1;
    fp->fnidx = fnidx;

    return 0;
}