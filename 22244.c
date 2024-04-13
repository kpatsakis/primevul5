EXPORTED int annotate_state_set_server(annotate_state_t *state)
{
    return annotate_state_set_scope(state, NULL, NULL, 0);
}