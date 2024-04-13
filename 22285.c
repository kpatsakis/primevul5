EXPORTED int annotate_state_writemask(annotate_state_t *state,
                                      const char *entry,
                                      const char *userid,
                                      const struct buf *value)
{
    /* if the user is the owner, then write to the shared namespace */
    if (mboxname_userownsmailbox(userid, state->mailbox->name))
        return annotate_state_write(state, entry, "", value);
    else
        return annotate_state_write(state, entry, userid, value);
}