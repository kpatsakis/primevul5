static int make_entry(struct buf *data,
                      const struct buf *value,
                      modseq_t modseq,
                      unsigned char flags)
{
    unsigned long l;
    static const char contenttype[] = "text/plain"; /* fake */
    unsigned long long nmodseq;

    /* Make sure that native types are wide enough */
    assert(sizeof(modseq_t) <= sizeof(unsigned long long));
    nmodseq = htonll((unsigned long long) modseq);

    l = htonl(value->len);
    buf_appendmap(data, (const char *)&l, sizeof(l));

    buf_appendmap(data, value->s ? value->s : "", value->len);
    buf_putc(data, '\0');

    /*
     * Older versions of Cyrus expected content-type and
     * modifiedsince fields after the value.  We don't support those
     * but we write out default values just in case the database
     * needs to be read by older versions of Cyrus
     */
    buf_appendcstr(data, contenttype);
    buf_putc(data, '\0');

    l = 0;  /* fake modifiedsince */
    buf_appendmap(data, (const char *)&l, sizeof(l));

    /* Append modseq at the end */
    buf_appendmap(data, (const char *)&nmodseq, sizeof(nmodseq));

    /* Append flags */
    buf_putc(data, flags);

    return 0;
}