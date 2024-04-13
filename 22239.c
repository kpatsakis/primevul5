static int annotate_state_set_scope(annotate_state_t *state,
                                    const mbentry_t *mbentry,
                                    struct mailbox *mailbox,
                                    unsigned int uid)
{
    int r = 0;
    annotate_db_t *oldd = NULL;
    int oldwhich = state->which;

    init_internal();

    /* Carefully preserve the reference on the old DB just in case it
     * turns out to be the same as the new DB, so we avoid the overhead
     * of an unnecessary cyrusdb_open/close pair. */
    oldd = state->d;
    state->d = NULL;

    annotate_state_unset_scope(state);

    if (mbentry) {
        assert(!mailbox);
        assert(!uid);
        if (!mbentry->server) {
            /* local mailbox */
            r = mailbox_open_iwl(mbentry->name, &mailbox);
            if (r)
                goto out;
            state->ourmailbox = mailbox;
        }
        state->mbentry = mbentry;
        state->which = ANNOTATION_SCOPE_MAILBOX;
    }

    else if (uid) {
        assert(mailbox);
        state->which = ANNOTATION_SCOPE_MESSAGE;
    }
    else if (mailbox) {
        assert(!uid);
        state->which = ANNOTATION_SCOPE_MAILBOX;
    }
    else {
        assert(!mailbox);
        assert(!uid);
        state->which = ANNOTATION_SCOPE_SERVER;
    }
    assert(oldwhich == ANNOTATION_SCOPE_UNKNOWN ||
           oldwhich == state->which);
    state->mailbox = mailbox;
    state->uid = uid;

    r = _annotate_getdb(mailbox ? mailbox->name : NULL, uid,
                        CYRUSDB_CREATE, &state->d);

out:
    annotate_putdb(&oldd);
    return r;
}