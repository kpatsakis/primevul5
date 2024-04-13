static void annotate_closedb(annotate_db_t *d)
{
    annotate_db_t *dx, *prev = NULL;
    int r;

    /* detach from the global list */
    for (dx = all_dbs_head ; dx && dx != d ; prev = dx, dx = dx->next)
        ;
    assert(dx);
    assert(d == dx);
    detach_db(prev, d);

#if DEBUG
    syslog(LOG_ERR, "Closing annotations db %s\n", d->filename);
#endif

    r = cyrusdb_close(d->db);
    if (r)
        syslog(LOG_ERR, "DBERROR: error closing annotations %s: %s",
               d->filename, cyrusdb_strerror(r));

    free(d->filename);
    free(d->mboxname);
    memset(d, 0, sizeof(*d));   /* JIC */
    free(d);
}