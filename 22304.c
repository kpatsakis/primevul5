HIDDEN int annotate_state_set_message(annotate_state_t *state,
                               struct mailbox *mailbox,
                               unsigned int uid)
{
    return annotate_state_set_scope(state, NULL, mailbox, uid);
}