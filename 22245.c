static void annotation_get_usermodseq(annotate_state_t *state,
                                      struct annotate_entry_list *entry)
{
    struct buf value = BUF_INITIALIZER;
    struct mboxname_counters counters;
    char *mboxname = NULL;

    memset(&counters, 0, sizeof(struct mboxname_counters));

    assert(state);
    assert(state->userid);

    mboxname = mboxname_user_mbox(state->userid, NULL);
    mboxname_read_counters(mboxname, &counters);

    buf_printf(&value, "%llu", counters.highestmodseq);

    output_entryatt(state, entry->name, state->userid, &value);
    free(mboxname);
    buf_free(&value);
}