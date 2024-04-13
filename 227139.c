int HTS_RESULT_USED bcf_hdr_parse_sample_line(bcf_hdr_t *h, const char *str)
{
    int ret = 0;
    int i = 0;
    const char *p, *q;
    // add samples
    for (p = q = str;; ++q) {
        if (*q > '\n') continue;
        if (++i > 9) {
            if ( bcf_hdr_add_sample_len(h, p, q - p) < 0 ) ret = -1;
        }
        if (*q == 0 || *q == '\n' || ret < 0) break;
        p = q + 1;
    }

    return ret;
}