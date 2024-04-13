static int _annotate_rewrite(struct mailbox *oldmailbox,
                             uint32_t olduid,
                             const char *olduserid,
                             struct mailbox *newmailbox,
                             uint32_t newuid,
                             const char *newuserid,
                             int copy)
{
    struct rename_rock rrock;

    rrock.oldmailbox = oldmailbox;
    rrock.newmailbox = newmailbox;
    rrock.olduserid = olduserid;
    rrock.newuserid = newuserid;
    rrock.olduid = olduid;
    rrock.newuid = newuid;
    rrock.copy = copy;

    return annotatemore_findall(oldmailbox->name, olduid, "*", /*modseq*/0,
                                &rename_cb, &rrock, /*flags*/0);
}