EXPORTED int annotate_state_writemdata(annotate_state_t *state,
                                       const char *entry,
                                       const char *userid,
                                       const struct buf *value,
                                       const struct annotate_metadata *mdata)
{
    return write_entry(state->mailbox, state->uid, entry, userid, value,
                       /*ignorequota*/1, 0, mdata, /*maywrite*/1);
}