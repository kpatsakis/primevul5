static void annotation_get_lastpop(annotate_state_t *state,
                                   struct annotate_entry_list *entry)
{
    struct mailbox *mailbox = state->mailbox;
    char valuebuf[RFC3501_DATETIME_MAX+1];
    struct buf value = BUF_INITIALIZER;

    assert(mailbox);

    if (mailbox->i.pop3_last_login) {
        time_to_rfc3501(mailbox->i.pop3_last_login, valuebuf,
                        sizeof(valuebuf));
        buf_appendcstr(&value, valuebuf);
    }

    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}