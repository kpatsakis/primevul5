static int annotation_set_todb(annotate_state_t *state,
                               struct annotate_entry_list *entry,
                               int maywrite)
{
    int r = 0;

    if (entry->have_shared)
        r = write_entry(state->mailbox, state->uid,
                        entry->name, "",
                        &entry->shared, 0, state->silent, NULL, maywrite);
    if (!r && entry->have_priv)
        r = write_entry(state->mailbox, state->uid,
                        entry->name, state->userid,
                        &entry->priv, 0, state->silent, NULL, maywrite);

    return r;
}