EXPORTED int annotate_state_writesilent(annotate_state_t *state,
                                        const char *entry,
                                        const char *userid,
                                        const struct buf *value)
{
    return write_entry(state->mailbox, state->uid,
                       entry, userid, value, /*ignorequota*/1,
                       /*silent*/1, NULL, /*maywrite*/1);
}