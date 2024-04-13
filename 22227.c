static int _annotate_getdb(const char *mboxname,
                           unsigned int uid,
                           int dbflags,
                           annotate_db_t **dbp)
{
    annotate_db_t *d, *prev = NULL;
    char *fname = NULL;
    struct db *db;
    int r;

    *dbp = NULL;

    /*
     * The incoming (mboxname,uid) tuple tells us which scope we
     * need a database for.  Translate into the mboxname used to
     * key annotate_db_t's, which is slightly different: message
     * scope goes into a per-mailbox db, others in the global db.
     */
    if (!strcmpsafe(mboxname, NULL) /*server scope*/ ||
        !uid /* mailbox scope*/)
        mboxname = NULL;

    /* try to find an existing db for the mbox */
    for (d = all_dbs_head ; d ; prev = d, d = d->next) {
        if (!strcmpsafe(mboxname, d->mboxname)) {
            /* found it, bump the refcount */
            d->refcount++;
            *dbp = d;
            /*
             * Splay the db_t to the end of the global list.
             * This ensures the list remains in getdb() call
             * order, and in particular that the dbs are
             * committed in getdb() call order.  This is
             * necessary to ensure safety should a commit fail
             * while moving annotations between per-mailbox dbs
             */
            detach_db(prev, d);
            append_db(d);
            return 0;
        }
    }
    /* not found, open/create a new one */

    r = annotate_dbname(mboxname, &fname);
    if (r)
        goto error;
#if DEBUG
    syslog(LOG_ERR, "Opening annotations db %s\n", fname);
#endif

    r = cyrusdb_open(DB, fname, dbflags | CYRUSDB_CONVERT, &db);
    if (r != 0) {
        if (!(dbflags & CYRUSDB_CREATE) && r == CYRUSDB_NOTFOUND)
            goto error;
        syslog(LOG_ERR, "DBERROR: opening %s: %s",
                        fname, cyrusdb_strerror(r));
        goto error;
    }

    /* record all the above */
    d = xzmalloc(sizeof(*d));
    d->refcount = 1;
    d->mboxname = xstrdupnull(mboxname);
    d->filename = fname;
    d->db = db;

    append_db(d);

    *dbp = d;
    return 0;

error:
    free(fname);
    *dbp = NULL;
    return r;
}