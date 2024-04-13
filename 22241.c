static void annotation_get_partition(annotate_state_t *state,
                                     struct annotate_entry_list *entry)
{
    struct buf value = BUF_INITIALIZER;
    int r;

    assert(state);
    assert(state->which == ANNOTATION_SCOPE_MAILBOX);
    r = annotate_state_need_mbentry(state);
    assert(r == 0);

    /* Make sure its a local mailbox */
    if (state->mbentry->server) goto out;

    /* Check ACL */
    if (!state->isadmin &&
        (!state->mbentry->acl ||
         !(cyrus_acl_myrights(state->auth_state, state->mbentry->acl) & ACL_LOOKUP)))
        goto out;

    buf_appendcstr(&value, state->mbentry->partition);

    output_entryatt(state, entry->name, "", &value);
out:
    buf_free(&value);
}