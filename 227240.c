int bcf_readrec(BGZF *fp, void *null, void *vv, int *tid, hts_pos_t *beg, hts_pos_t *end)
{
    bcf1_t *v = (bcf1_t *) vv;
    int ret = bcf_read1_core(fp, v);
    if (ret == 0) ret = bcf_record_check(NULL, v);
    if (ret  >= 0)
        *tid = v->rid, *beg = v->pos, *end = v->pos + v->rlen;
    return ret;
}