EXPORTED int annotatemore_msg_lookup(const char *mboxname, uint32_t uid, const char *entry,
                                     const char *userid, struct buf *value)
{
    char key[MAX_MAILBOX_PATH+1];
    size_t keylen, datalen;
    int r;
    const char *data;
    annotate_db_t *d = NULL;
    struct annotate_metadata mdata;

    init_internal();

    r = _annotate_getdb(mboxname, uid, 0, &d);
    if (r)
        return (r == CYRUSDB_NOTFOUND ? 0 : r);

    keylen = make_key(mboxname, uid, entry, userid, key, sizeof(key));

    do {
        r = cyrusdb_fetch(d->db, key, keylen, &data, &datalen, tid(d));
    } while (r == CYRUSDB_AGAIN);

    if (!r && data) {
        r = split_attribs(data, datalen, value, &mdata);
        if (!r) {
            /* Force a copy, in case the putdb() call destroys
             * the per-db data area that @data points to.  */
            buf_cstring(value);
        }
        if (mdata.flags & ANNOTATE_FLAG_DELETED) {
            buf_free(value);
            r = CYRUSDB_NOTFOUND;
        }
    }
    if (r == CYRUSDB_NOTFOUND) r = 0;

    annotate_putdb(&d);
    return r;
}