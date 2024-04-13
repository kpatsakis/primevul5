static int split_attribs(const char *data, int datalen,
                         struct buf *value, struct annotate_metadata *mdata)
{
    unsigned long tmp; /* for alignment */
    const char *tmps;
    const char *end = data + datalen;

    /* initialize metadata */
    memset(mdata, 0, sizeof(struct annotate_metadata));

    /* xxx sanity check the data? */
    if (datalen <= 0)
            return 1;
    /*
     * Sigh...this is dumb.  We take care to be machine independent by
     * storing the length in network byte order...but the size of the
     * length field depends on whether we're running on a 32b or 64b
     * platform.
     */
    memcpy(&tmp, data, sizeof(unsigned long));
    data += sizeof(unsigned long); /* skip to value */

    buf_init_ro(value, data, ntohl(tmp));

    /*
     * In records written by older versions of Cyrus, there will be
     * binary encoded content-type and modifiedsince values after the
     * data. We don't care about those anymore, so we just ignore them
     * and skip to the entry's metadata.
     */
    tmps = data + ntohl(tmp) + 1;  /* Skip zero-terminated value */
    if (tmps < end) {
        tmps += strlen(tmps) + 1;      /* Skip zero-terminated content-type */
        tmps += sizeof(unsigned long); /* Skip modifiedsince value */
    }

    if (tmps < end) {
        /* make sure ntohll's input is correctly aligned */
        modseq_t modseq;
        memcpy(&modseq, tmps, sizeof(modseq));
        mdata->modseq = ntohll(modseq);
        tmps += sizeof(modseq_t);
    }

    if (tmps < end) {
        mdata->flags = *tmps;
        tmps++;
    }

    /* normalise deleted entries */
    if (mdata->flags & ANNOTATE_FLAG_DELETED) {
        buf_reset(value);
    }

    return 0;
}