EXPORTED int annotatemore_rawwrite(const char *mboxname, const char *entry,
                                   const char *userid, const struct buf *value)
{
    char key[MAX_MAILBOX_PATH+1];
    int keylen, r;
    annotate_db_t *d = NULL;
    uint32_t uid = 0;

    init_internal();

    r = _annotate_getdb(mboxname, uid, CYRUSDB_CREATE, &d);
    if (r) goto done;

    /* must be in a transaction to modify the db */
    annotate_begin(d);

    keylen = make_key(mboxname, uid, entry, userid, key, sizeof(key));

    if (value->s == NULL) {
        do {
            r = cyrusdb_delete(d->db, key, keylen, tid(d), /*force*/1);
        } while (r == CYRUSDB_AGAIN);
    }
    else {
        struct buf data = BUF_INITIALIZER;

        make_entry(&data, value, uid, /*flags*/0);

        do {
            r = cyrusdb_store(d->db, key, keylen, data.s, data.len, tid(d));
        } while (r == CYRUSDB_AGAIN);
        buf_free(&data);
    }

    if (r) goto done;
    r = annotate_commit(d);

done:
    annotate_putdb(&d);

    return r;
}