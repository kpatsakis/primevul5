static int annotate_commit(annotate_db_t *d)
{
    int r = 0;

    /* silently succeed if not in a txn */
    if (!d || !d->in_txn) return 0;

    if (d->txn) {
#if DEBUG
        syslog(LOG_ERR, "Committing annotations db %s\n", d->filename);
#endif
        r = cyrusdb_commit(d->db, d->txn);
        if (r)
            r = IMAP_IOERROR;
        d->txn = NULL;
    }
    d->in_txn = 0;

    return r;
}