int bcf_index_build3(const char *fn, const char *fnidx, int min_shift, int n_threads)
{
    htsFile *fp;
    hts_idx_t *idx;
    tbx_t *tbx;
    int ret;
    if ((fp = hts_open(fn, "rb")) == 0) return -2;
    if (n_threads)
        hts_set_threads(fp, n_threads);
    if ( fp->format.compression!=bgzf ) { hts_close(fp); return -3; }
    switch (fp->format.format) {
        case bcf:
            if (!min_shift) {
                hts_log_error("TBI indices for BCF files are not supported");
                ret = -1;
            } else {
                idx = bcf_index(fp, min_shift);
                if (idx) {
                    ret = hts_idx_save_as(idx, fn, fnidx, HTS_FMT_CSI);
                    if (ret < 0) ret = -4;
                    hts_idx_destroy(idx);
                }
                else ret = -1;
            }
            break;

        case vcf:
            tbx = tbx_index(hts_get_bgzfp(fp), min_shift, &tbx_conf_vcf);
            if (tbx) {
                ret = hts_idx_save_as(tbx->idx, fn, fnidx, min_shift > 0 ? HTS_FMT_CSI : HTS_FMT_TBI);
                if (ret < 0) ret = -4;
                tbx_destroy(tbx);
            }
            else ret = -1;
            break;

        default:
            ret = -3;
            break;
    }
    hts_close(fp);
    return ret;
}