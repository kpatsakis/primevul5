static int vcf_idx_init(htsFile *fp, bcf_hdr_t *h, int min_shift, const char *fnidx) {
    int n_lvls, fmt;

    if (min_shift == 0) {
        min_shift = 14;
        n_lvls = 5;
        fmt = HTS_FMT_TBI;
    } else {
        // Set initial n_lvls to match tbx_index()
        int starting_n_lvls = (TBX_MAX_SHIFT - min_shift + 2) / 3;
        // Increase if necessary
        n_lvls = idx_calc_n_lvls_ids(h, min_shift, starting_n_lvls, NULL);
        fmt = HTS_FMT_CSI;
    }

    fp->idx = hts_idx_init(0, fmt, bgzf_tell(fp->fp.bgzf), min_shift, n_lvls);
    if (!fp->idx) return -1;

    // Tabix meta data, added even in CSI for VCF
    uint8_t conf[4*7];
    u32_to_le(TBX_VCF, conf+0);  // fmt
    u32_to_le(1,       conf+4);  // name col
    u32_to_le(2,       conf+8);  // beg col
    u32_to_le(0,       conf+12); // end col
    u32_to_le('#',     conf+16); // comment
    u32_to_le(0,       conf+20); // n.skip
    u32_to_le(0,       conf+24); // ref name len
    if (hts_idx_set_meta(fp->idx, sizeof(conf)*sizeof(*conf), (uint8_t *)conf, 1) < 0) {
        hts_idx_destroy(fp->idx);
        fp->idx = NULL;
        return -1;
    }
    fp->fnidx = fnidx;

    return 0;
}