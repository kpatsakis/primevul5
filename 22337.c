static int rename_cb(const char *mboxname __attribute__((unused)),
                     uint32_t uid,
                     const char *entry,
                     const char *userid, const struct buf *value,
                     const struct annotate_metadata *mdata __attribute__((unused)),
                     void *rock)
{
    struct rename_rock *rrock = (struct rename_rock *) rock;
    int r = 0;

    if (rrock->newmailbox &&
            /* snoozed MUST only appear on one copy of a message */
            strcmp(entry, IMAP_ANNOT_NS "snoozed") &&
            /* displayname stores the UTF-8 encoded JMAP name of a mailbox */
            strcmp(entry, IMAP_ANNOT_NS "displayname")) {
        /* create newly renamed entry */
        const char *newuserid = userid;

        if (rrock->olduserid && rrock->newuserid &&
            !strcmpsafe(rrock->olduserid, userid)) {
            /* renaming a user, so change the userid for priv annots */
            newuserid = rrock->newuserid;
        }
        r = write_entry(rrock->newmailbox, rrock->newuid, entry, newuserid,
                        value, /*ignorequota*/0, /*silent*/0, NULL, /*maywrite*/1);
    }

    if (!rrock->copy && !r) {
        /* delete existing entry */
        struct buf dattrib = BUF_INITIALIZER;
        r = write_entry(rrock->oldmailbox, uid, entry, userid, &dattrib,
                        /*ignorequota*/0, /*silent*/0, NULL, /*maywrite*/1);
    }

    return r;
}