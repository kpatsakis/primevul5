EXPORTED int annotatemore_writemask(const char *mboxname, const char *entry,
                                    const char *userid, const struct buf *value)
{
    if (mboxname_userownsmailbox(userid, mboxname))
        return annotatemore_write(mboxname, entry, "", value);
    else
        return annotatemore_write(mboxname, entry, userid, value);
}