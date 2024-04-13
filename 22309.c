static int _annotate_may_store(annotate_state_t *state,
                               int is_shared,
                               const annotate_entrydesc_t *desc)
{
    unsigned int my_rights;
    unsigned int needed = 0;
    const char *acl = NULL;

    /* Admins can do anything */
    if (state->isadmin)
        return 1;

    if (state->which == ANNOTATION_SCOPE_SERVER) {
        /* RFC5464 doesn't mention access control for server
         * annotations, but this seems a sensible practice and is
         * consistent with past Cyrus behaviour */
        return !is_shared;
    }
    else if (state->which == ANNOTATION_SCOPE_MAILBOX) {
        assert(state->mailbox);

        /* Make sure its a local mailbox annotation */
        if (state->mbentry && state->mbentry->server)
            return 0;

        acl = state->mailbox->acl;
        /* RFC5464 is a trifle vague about access control for mailbox
         * annotations but this seems to be compliant */
        needed = ACL_LOOKUP;
        if (is_shared)
            needed |= ACL_READ|ACL_WRITE|desc->extra_rights;
        /* fall through to ACL check */
    }
    else if (state->which == ANNOTATION_SCOPE_MESSAGE) {
        assert(state->mailbox);
        acl = state->mailbox->acl;
        /* RFC5257: writing to a private annotation needs 'r'.
         * Writing to a shared annotation needs 'n' */
        needed = (is_shared ? ACL_ANNOTATEMSG : ACL_READ);
        /* fall through to ACL check */
    }

    if (!acl)
        return 0;

    my_rights = cyrus_acl_myrights(state->auth_state, acl);

    return ((my_rights & needed) == needed);
}