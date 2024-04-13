EXPORTED void annotate_state_begin(annotate_state_t *state)
{
    init_internal();

    annotate_begin(state->d);
}