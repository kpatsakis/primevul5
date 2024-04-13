EXPORTED int annotate_state_set_mailbox_mbe(annotate_state_t *state,
                                   const mbentry_t *mbentry)
{
    return annotate_state_set_scope(state, mbentry, NULL, 0);
}