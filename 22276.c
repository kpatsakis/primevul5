static void annotate_state_free(annotate_state_t **statep)
{
    annotate_state_t *state = *statep;

    if (!state)
        return;

    annotate_state_finish(state);
    annotate_state_unset_scope(state);
    free(state);
    *statep = NULL;
}