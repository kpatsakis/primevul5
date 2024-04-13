static int annotation_set_specialuse(annotate_state_t *state,
                                     struct annotate_entry_list *entry,
                                     int maywrite)
{
    struct buf res = BUF_INITIALIZER;
    int r = IMAP_PERMISSION_DENIED;

    assert(state->mailbox);

    /* Effectively removes the annotation */
    if (entry->priv.s == NULL) {
        r = write_entry(state->mailbox, state->uid, entry->name, state->userid,
                        &entry->priv, /*ignorequota*/0, /*silent*/0, NULL, maywrite);
        goto done;
    }

    r = specialuse_validate(state->mailbox->name, state->userid,
                            buf_cstring(&entry->priv), &res);
    if (r) goto done;

    r = write_entry(state->mailbox, state->uid, entry->name, state->userid,
                    &res, /*ignorequota*/0, state->silent, NULL, maywrite);

done:
    buf_free(&res);

    return r;
}