static int read_old_value(annotate_db_t *d,
                          const char *key, int keylen,
                          struct buf *valp,
                          struct annotate_metadata *mdata)
{
    int r;
    size_t datalen;
    const char *data;

    do {
        r = cyrusdb_fetch(d->db, key, keylen, &data, &datalen, tid(d));
    } while (r == CYRUSDB_AGAIN);

    if (r == CYRUSDB_NOTFOUND) {
        r = 0;
        goto out;
    }
    if (r || !data)
        goto out;

    r = split_attribs(data, datalen, valp, mdata);

out:
    return r;
}