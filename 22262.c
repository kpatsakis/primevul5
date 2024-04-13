EXPORTED int annotatemore_msg_lookupmask(const char *mboxname, uint32_t uid, const char *entry,
                                         const char *userid, struct buf *value)
{
    int r = 0;
    value->len = 0; /* just in case! */

    init_internal();

    /* only if the user isn't the owner, we look for a masking value */
    if (!mboxname_userownsmailbox(userid, mboxname))
        r = annotatemore_msg_lookup(mboxname, uid, entry, userid, value);
    /* and if there isn't one, we fall through to the shared value */
    if (value->len == 0)
        r = annotatemore_msg_lookup(mboxname, uid, entry, "", value);
    /* and because of Bron's use of NULL rather than "" at FastMail... */
    if (value->len == 0)
        r = annotatemore_msg_lookup(mboxname, uid, entry, NULL, value);
    return r;
}