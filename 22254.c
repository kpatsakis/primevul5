static int _annotate_store_entries(annotate_state_t *state)
{
    struct annotate_entry_list *ee;
    int r = 0;
    unsigned oldsilent = state->silent;

    /* Loop through the list of provided entries to set */
    for (ee = state->entry_list ; ee ; ee = ee->next) {
        int maystore = 1;

        /* Skip annotations that can't be stored on frontend */
        if ((ee->desc->proxytype == BACKEND_ONLY) &&
            (state->mbentry && state->mbentry->server))
            continue;

        if (ee->have_shared &&
            !_annotate_may_store(state, /*shared*/1, ee->desc)) {
            maystore = 0;
        }

        if (ee->have_priv &&
            !_annotate_may_store(state, /*shared*/0, ee->desc)) {
            maystore = 0;
        }

        r = ee->desc->set(state, ee, maystore);
        if (r)
            goto done;

        /* only the first write for message annotations isn't silent! */
        if (state->which == ANNOTATION_SCOPE_MESSAGE)
            state->silent = 1;
    }

done:
    state->silent = oldsilent;
    return r;
}