static int mlookup(const char *name, struct mboxlist_entry **mbentryptr)
{
    struct mboxlist_entry *mbentry = NULL;
    int r;
    
    r = mboxlist_lookup(name, &mbentry, NULL);
    if (r == IMAP_MAILBOX_NONEXISTENT && config_mupdate_server) {
	kick_mupdate();
	mboxlist_entry_free(&mbentry);
	r = mboxlist_lookup(name, &mbentry, NULL);
    }

    if (mbentryptr && !r) *mbentryptr = mbentry;
    else mboxlist_entry_free(&mbentry);

    return r;
}