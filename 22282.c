EXPORTED int annotate_rename_mailbox(struct mailbox *oldmailbox,
                            struct mailbox *newmailbox)
{
    /* rename one mailbox */
    char *olduserid = mboxname_to_userid(oldmailbox->name);
    char *newuserid = mboxname_to_userid(newmailbox->name);
    annotate_db_t *d = NULL;
    int r = 0;

    init_internal();

    /* rewrite any per-folder annotations from the global db */
    r = _annotate_getdb(NULL, 0, /*don't create*/0, &d);
    if (r == CYRUSDB_NOTFOUND) {
        /* no global database, must not be anything to rename */
        r = 0;
        goto done;
    }
    if (r) goto done;

    annotate_begin(d);

    /* copy here - delete will dispose of old records later */
    r = _annotate_rewrite(oldmailbox, 0, olduserid,
                          newmailbox, 0, newuserid,
                         /*copy*/1);
    if (r) goto done;

    r = annotate_commit(d);
    if (r) goto done;

    /*
     * The per-folder database got moved or linked by mailbox_copy_files().
     */

 done:
    annotate_putdb(&d);
    free(olduserid);
    free(newuserid);

    return r;
}