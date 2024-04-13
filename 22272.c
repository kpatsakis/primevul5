static void annotation_get_usercounters(annotate_state_t *state,
                                        struct annotate_entry_list *entry)
{
    struct buf value = BUF_INITIALIZER;
    struct mboxname_counters counters;
    char *mboxname = NULL;

    assert(state);
    assert(state->userid);

    mboxname = mboxname_user_mbox(state->userid, NULL);
    int r = mboxname_read_counters(mboxname, &counters);

    if (!r) buf_printf(&value, "%u %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %u",
                       counters.version, counters.highestmodseq,
                       counters.mailmodseq, counters.caldavmodseq,
                       counters.carddavmodseq, counters.notesmodseq,
                       counters.mailfoldersmodseq, counters.caldavfoldersmodseq,
                       counters.carddavfoldersmodseq, counters.notesfoldersmodseq,
                       counters.quotamodseq, counters.raclmodseq,
                       counters.uidvalidity);

    output_entryatt(state, entry->name, state->userid, &value);
    free(mboxname);
    buf_free(&value);
}