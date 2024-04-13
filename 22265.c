static void annotation_get_foldermodseq(annotate_state_t *state,
                                        struct annotate_entry_list *entry)
{
    struct buf value = BUF_INITIALIZER;

    assert(state);
    annotate_state_need_mbentry(state);
    assert(state->mbentry);

    buf_printf(&value, "%llu", state->mbentry->foldermodseq);
    output_entryatt(state, entry->name, "", &value);

    buf_free(&value);
}