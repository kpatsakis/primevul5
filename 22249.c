EXPORTED int annotate_state_set_mailbox(annotate_state_t *state,
                                struct mailbox *mailbox)
{
    return annotate_state_set_scope(state, NULL, mailbox, 0);
}