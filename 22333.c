EXPORTED int annotate_msg_copy(struct mailbox *oldmailbox, uint32_t olduid,
                      struct mailbox *newmailbox, uint32_t newuid,
                      const char *userid)
{
    annotate_db_t *d = NULL;
    int r;

    init_internal();

    r = _annotate_getdb(newmailbox->name, newuid, CYRUSDB_CREATE, &d);
    if (r) return r;

    annotate_begin(d);

    /* If these are not true, nobody will ever commit the data we're
     * about to copy, and that would be sad */
    assert(newmailbox->annot_state != NULL);
    assert(newmailbox->annot_state->d == d);

    r = _annotate_rewrite(oldmailbox, olduid, userid,
                          newmailbox, newuid, userid,
                          /*copy*/1);

    annotate_putdb(&d);
    return r;
}