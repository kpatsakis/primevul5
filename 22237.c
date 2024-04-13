HIDDEN void annotate_putdb(annotate_db_t **dbp)
{
    annotate_db_t *d;

    if (!dbp || !(d = *dbp))
        return;
    assert(d->refcount > 0);
    if (--d->refcount == 0) {
        if (d->in_txn && d->txn) {
            syslog(LOG_ERR, "IOERROR: dropped last reference on "
                            "database %s with uncommitted updates, "
                            "aborting - DATA LOST!",
                            d->filename);
            annotate_abort(d);
        }
        assert(!d->in_txn);
        annotate_closedb(d);
    }
    *dbp = NULL;
}