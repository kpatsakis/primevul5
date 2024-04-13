EXPORTED int annotatemore_lookupmask(const char *mboxname, const char *entry,
                                     const char *userid, struct buf *value)
{
    return annotatemore_msg_lookupmask(mboxname, /*uid*/0, entry, userid, value);
}