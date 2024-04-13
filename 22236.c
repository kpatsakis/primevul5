EXPORTED int annotate_state_store(annotate_state_t *state, struct entryattlist *l)
{
    int r = 0;
    struct entryattlist *e = l;
    struct attvaluelist *av;

    annotate_state_start(state);

    /* Build a list of callbacks for storing the annotations */
    while (e) {
        int attribs;
        const annotate_entrydesc_t *desc = NULL;
        struct annotate_entry_list *nentry = NULL;

        /* See if we support this entry */
        r = find_desc_store(state, e->entry, &desc);
        if (r)
            goto cleanup;

        /* Add this entry to our list only if it
           applies to our particular server type */
        if ((desc->proxytype != PROXY_ONLY)
            || proxy_store_func)
            nentry = _annotate_state_add_entry(state, desc, e->entry);

        /* See if we are allowed to set the given attributes. */
        attribs = desc->attribs;
        av = e->attvalues;
        while (av) {
            if (!strcmp(av->attrib, "value.shared")) {
                if (!(attribs & ATTRIB_VALUE_SHARED)) {
                    r = IMAP_PERMISSION_DENIED;
                    goto cleanup;
                }
                r = annotate_canon_value(&av->value,
                                         desc->type);
                if (r)
                    goto cleanup;
                if (nentry) {
                    buf_init_ro(&nentry->shared, av->value.s, av->value.len);
                    nentry->have_shared = 1;
                }
            }
            else if (!strcmp(av->attrib, "content-type.shared") ||
                     !strcmp(av->attrib, "content-type.priv")) {
                syslog(LOG_WARNING, "annotatemore_store: client used "
                                    "deprecated attribute \"%s\", ignoring",
                                    av->attrib);
            }
            else if (!strcmp(av->attrib, "value.priv")) {
                if (!(attribs & ATTRIB_VALUE_PRIV)) {
                    r = IMAP_PERMISSION_DENIED;
                    goto cleanup;
                }
                r = annotate_canon_value(&av->value,
                                         desc->type);
                if (r)
                    goto cleanup;
                if (nentry) {
                    buf_init_ro(&nentry->priv, av->value.s, av->value.len);
                    nentry->have_priv = 1;
                }
            }
            else {
                r = IMAP_PERMISSION_DENIED;
                goto cleanup;
            }

            av = av->next;
        }

        e = e->next;
    }

    if (state->which == ANNOTATION_SCOPE_SERVER) {
        r = _annotate_store_entries(state);
    }

    else if (state->which == ANNOTATION_SCOPE_MAILBOX) {
        if (proxy_store_func) {
            r = annotate_state_need_mbentry(state);
            if (r)
                goto cleanup;
            assert(state->mbentry);
        }
        else assert(state->mailbox);

        r = _annotate_store_entries(state);
        if (r)
            goto cleanup;

        state->count++;

        if (proxy_store_func && state->mbentry->server &&
            !hash_lookup(state->mbentry->server, &state->server_table)) {
            hash_insert(state->mbentry->server, (void *)0xDEADBEEF, &state->server_table);
        }

        if (!r && !state->count) r = IMAP_MAILBOX_NONEXISTENT;

        if (proxy_store_func) {
            if (!r) {
                /* proxy command to backends */
                struct proxy_rock prock = { NULL, NULL };
                prock.mbox_pat = state->mbentry->ext_name;
                prock.entryatts = l;
                hash_enumerate(&state->server_table, store_proxy, &prock);
            }
        }
    }
    else if (state->which == ANNOTATION_SCOPE_MESSAGE) {
        r = _annotate_store_entries(state);
        if (r)
            goto cleanup;
    }

cleanup:
    annotate_state_finish(state);
    return r;
}