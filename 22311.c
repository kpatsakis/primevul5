EXPORTED int annotate_delete_mailbox(struct mailbox *mailbox)
{
    int r = 0;
    char *fname = NULL;
    annotate_db_t *d = NULL;

    init_internal();

    assert(mailbox);

    /* remove any per-folder annotations from the global db */
    r = _annotate_getdb(NULL, 0, /*don't create*/0, &d);
    if (r == CYRUSDB_NOTFOUND) {
        /* no global database, must not be anything to rename */
        r = 0;
        goto out;
    }
    if (r) goto out;

    annotate_begin(d);

    r = _annotate_rewrite(mailbox,
                          /*olduid*/0, /*olduserid*/NULL,
                          /*newmailbox*/NULL,
                          /*newuid*/0, /*newuserid*/NULL,
                          /*copy*/0);
    if (r) goto out;

    /* remove the entire per-folder database */
    r = annotate_dbname_mailbox(mailbox, &fname);
    if (r) goto out;

    /* (gnb)TODO: do we even need to do this?? */
    if (unlink(fname) < 0 && errno != ENOENT) {
        syslog(LOG_ERR, "cannot unlink %s: %m", fname);
    }

    r = annotate_commit(d);

out:
    annotate_putdb(&d);
    free(fname);
    return r;
}