static void annotation_get_mailboxopt(annotate_state_t *state,
                                      struct annotate_entry_list *entry)
{
    struct mailbox *mailbox = state->mailbox;
    uint32_t flag = (unsigned long)entry->desc->rock;
    struct buf value = BUF_INITIALIZER;

    assert(mailbox);

    buf_appendcstr(&value,
                   (mailbox->i.options & flag ? "true" : "false"));
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}