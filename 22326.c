static void flush_entryatt(annotate_state_t *state)
{
    if (!state->attvalues)
        return;     /* nothing to flush */

    state->callback(state->lastname,
                    state->lastuid,
                    state->lastentry,
                    state->attvalues,
                    state->callback_rock);
    freeattvalues(state->attvalues);
    state->attvalues = NULL;
}