EXPORTED int annotate_state_fetch(annotate_state_t *state,
                         const strarray_t *entries, const strarray_t *attribs,
                         annotate_fetch_cb_t callback, void *rock)
{
    int i;
    struct glob *g;
    const ptrarray_t *non_db_entries;
    const annotate_entrydesc_t *db_entry;
    int r = 0;

    init_internal();

    annotate_state_start(state);
    state->callback = callback;
    state->callback_rock = rock;

    /* Build list of attributes to fetch */
    for (i = 0 ; i < attribs->count ; i++)
    {
        const char *s = attribs->data[i];
        int attribcount;

        /*
         * TODO: this is bogus.  The * and % wildcard characters applied
         * to attributes in the early drafts of the ANNOTATEMORE
         * extension, but not in later drafts where those characters are
         * actually illegal in attribute names.
         */
        g = glob_init(s, '.');

        for (attribcount = 0;
             annotation_attributes[attribcount].name;
             attribcount++) {
            if (GLOB_MATCH(g, annotation_attributes[attribcount].name)) {
                if (annotation_attributes[attribcount].entry & ATTRIB_DEPRECATED) {
                    if (strcmp(s, "*"))
                        syslog(LOG_WARNING, "annotatemore_fetch: client used "
                                            "deprecated attribute \"%s\", ignoring",
                                            annotation_attributes[attribcount].name);
                }
                else
                    state->attribs |= annotation_attributes[attribcount].entry;
            }
        }

        glob_free(&g);
    }

    if (!state->attribs)
        goto out;

    if (state->which == ANNOTATION_SCOPE_SERVER) {
        non_db_entries = &server_entries;
        db_entry = &server_db_entry;
    }
    else if (state->which == ANNOTATION_SCOPE_MAILBOX) {
        non_db_entries = &mailbox_entries;
        db_entry = &mailbox_db_entry;
    }
    else if (state->which == ANNOTATION_SCOPE_MESSAGE) {
        non_db_entries = &message_entries;
        db_entry = &message_db_entry;
    }
    else {
        syslog(LOG_ERR, "IOERROR: unknown annotation scope %d", state->which);
        r = IMAP_INTERNAL;
        goto out;
    }

    /* Build a list of callbacks for fetching the annotations */
    for (i = 0 ; i < entries->count ; i++)
    {
        const char *s = entries->data[i];
        int j;
        int check_db = 0; /* should we check the db for this entry? */

        g = glob_init(s, '/');

        for (j = 0 ; j < non_db_entries->count ; j++) {
            const annotate_entrydesc_t *desc = non_db_entries->data[j];

            if (!desc->get)
                continue;

            if (GLOB_MATCH(g, desc->name)) {
                /* Add this entry to our list only if it
                   applies to our particular server type */
                if ((desc->proxytype != PROXY_ONLY)
                    || proxy_fetch_func)
                    _annotate_state_add_entry(state, desc, desc->name);
            }

            if (!strcmp(s, desc->name)) {
                /* exact match */
                if (desc->proxytype != PROXY_ONLY) {
                    state->orig_entry = entries;  /* proxy it */
                }
                break;
            }
        }

        if (j == non_db_entries->count) {
            /* no [exact] match */
            state->orig_entry = entries;  /* proxy it */
            check_db = 1;
        }

        /* Add the db entry to our list if only if it
           applies to our particular server type */
        if (check_db &&
            ((db_entry->proxytype != PROXY_ONLY) || proxy_fetch_func)) {
            /* Add the db entry to our list */
            _annotate_state_add_entry(state, db_entry, s);
        }

        glob_free(&g);
    }

    if (state->which == ANNOTATION_SCOPE_SERVER) {
        _annotate_fetch_entries(state, /*proxy_check*/1);
    }
    else if (state->which == ANNOTATION_SCOPE_MAILBOX) {

        if (state->entry_list || proxy_fetch_func) {
            if (proxy_fetch_func) {
                r = annotate_state_need_mbentry(state);
                if (r)
                    goto out;
                assert(state->mbentry);
            }

            if (proxy_fetch_func && state->orig_entry) {
                state->orig_mailbox = state->mbentry->name;
                state->orig_attribute = attribs;
            }

            _annotate_fetch_entries(state, /*proxy_check*/1);

            if (proxy_fetch_func && state->orig_entry && state->mbentry->server &&
                !hash_lookup(state->mbentry->server, &state->server_table)) {
                /* xxx ignoring result */
                proxy_fetch_func(state->mbentry->server, state->mbentry->ext_name,
                                 state->orig_entry, state->orig_attribute);
                hash_insert(state->mbentry->server, (void *)0xDEADBEEF, &state->server_table);
            }
        }
    }
    else if (state->which == ANNOTATION_SCOPE_MESSAGE) {
        _annotate_fetch_entries(state, /*proxy_check*/0);
    }

    /* Flush last cached entry in output_entryatt() */
    flush_entryatt(state);

out:
    annotate_state_finish(state);
    return r;
}