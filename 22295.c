static void annotation_get_uniqueid(annotate_state_t *state,
                                    struct annotate_entry_list *entry)
{
    struct buf value = BUF_INITIALIZER;

    assert(state->mailbox);

    if (state->mailbox->uniqueid)
        buf_appendcstr(&value, state->mailbox->uniqueid);

    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}