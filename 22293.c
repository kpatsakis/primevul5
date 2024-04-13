static void annotation_get_synccrcs(annotate_state_t *state,
                                    struct annotate_entry_list *entry)
{
    struct mailbox *mailbox = state->mailbox;
    struct buf value = BUF_INITIALIZER;

    assert(mailbox);

    buf_printf(&value, "%u %u", mailbox->i.synccrcs.basic,
                                mailbox->i.synccrcs.annot);

    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}