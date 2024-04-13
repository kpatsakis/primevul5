EXPORTED int annotate_state_write(annotate_state_t *state,
                                  const char *entry,
                                  const char *userid,
                                  const struct buf *value)
{
    return write_entry(state->mailbox, state->uid,
                       entry, userid, value, /*ignorequota*/1,
                       state->silent, NULL, /*maywrite*/1);
}