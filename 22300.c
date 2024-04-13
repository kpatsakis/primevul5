static void _annotate_fetch_entries(annotate_state_t *state,
                                    int proxy_check)
{
    struct annotate_entry_list *ee;

    /* Loop through the list of provided entries to get */
    for (ee = state->entry_list; ee; ee = ee->next) {

        if (proxy_check) {
            if (ee->desc->proxytype == BACKEND_ONLY &&
                proxy_fetch_func &&
                !config_getstring(IMAPOPT_PROXYSERVERS))
                continue;
        }

        if (!_annotate_may_fetch(state, ee->desc))
            continue;

        ee->desc->get(state, ee);
    }
}