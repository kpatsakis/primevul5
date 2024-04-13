HIDDEN int annotate_msg_cleanup(struct mailbox *mailbox, unsigned int uid)
{
    char key[MAX_MAILBOX_PATH+1];
    size_t keylen;
    int r = 0;
    annotate_db_t *d = NULL;

    assert(uid);

    r = _annotate_getdb(mailbox->name, uid, 0, &d);
    if (r) return r;

    /* must be in a transaction to modify the db */
    annotate_begin(d);

    /* If these are not true, nobody will ever commit the data we're
     * about to copy, and that would be sad */
    assert(mailbox->annot_state != NULL);
    assert(mailbox->annot_state->d == d);

    keylen = make_key(mailbox->name, uid, "", NULL, key, sizeof(key));

    r = cyrusdb_foreach(d->db, key, keylen, NULL, &cleanup_cb, d, tid(d));

    annotate_putdb(&d);
    return r;
}