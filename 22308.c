EXPORTED annotate_state_t *annotate_state_new(void)
{
    annotate_state_t *state;

    state = xzmalloc(sizeof(*state));
    state->which = ANNOTATION_SCOPE_UNKNOWN;

    return state;
}