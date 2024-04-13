HIDDEN int annotate_getdb(const char *mboxname, annotate_db_t **dbp)
{
    if (!mboxname || !*mboxname) {
        syslog(LOG_ERR, "IOERROR: annotate_getdb called with no mboxname");
        return IMAP_INTERNAL;   /* we don't return the global db */
    }
    /* synthetic UID '1' forces per-mailbox mode */
    return _annotate_getdb(mboxname, 1, CYRUSDB_CREATE, dbp);
}