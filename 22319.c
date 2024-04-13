static int rw_cb(const char *mailbox __attribute__((unused)),
                 uint32_t uid __attribute__((unused)),
                 const char *entry, const char *userid,
                 const struct buf *value,
                 const struct annotate_metadata *mdata __attribute__((unused)),
                 void *rock)
{
    annotate_state_t *state = (annotate_state_t *)rock;

    if (!userid[0] || !strcmp(userid, state->userid)) {
        output_entryatt(state, entry, userid, value);
    }

    return 0;
}