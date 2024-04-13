static void annotate_state_unset_scope(annotate_state_t *state)
{
    init_internal();

    if (state->ourmailbox)
        mailbox_close(&state->ourmailbox);
    state->mailbox = NULL;

    if (state->ourmbentry)
        mboxlist_entry_free(&state->ourmbentry);
    state->mbentry = NULL;

    state->uid = 0;
    state->which = ANNOTATION_SCOPE_UNKNOWN;
    annotate_putdb(&state->d);
}