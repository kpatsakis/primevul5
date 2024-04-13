static void annotate_abort(annotate_db_t *d)
{
    /* don't double-abort */
    if (!d || !d->in_txn) return;

    if (d->txn) {
#if DEBUG
        syslog(LOG_ERR, "Aborting annotations db %s\n", d->filename);
#endif
        cyrusdb_abort(d->db, d->txn);
    }
    d->txn = NULL;
    d->in_txn = 0;
}