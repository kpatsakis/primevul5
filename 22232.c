static int _annotate_may_fetch(annotate_state_t *state,
                               const annotate_entrydesc_t *desc)
{
    unsigned int my_rights;
    unsigned int needed = 0;
    const char *acl = NULL;

    /* Admins can do anything */
    if (state->isadmin)
        return 1;

    /* Some entries need to do their own access control */
    if ((desc->type & ATTRIB_NO_FETCH_ACL_CHECK))
        return 1;

    if (state->which == ANNOTATION_SCOPE_SERVER) {
        /* RFC5464 doesn't mention access control for server
         * annotations, but this seems a sensible practice and is
         * consistent with past Cyrus behaviour */
        return 1;
    }
    else if (state->which == ANNOTATION_SCOPE_MAILBOX) {
        assert(state->mailbox || state->mbentry);

        /* Make sure its a local mailbox annotation */
        if (state->mbentry && state->mbentry->server)
            return 0;

        if (state->mailbox) acl = state->mailbox->acl;
        else if (state->mbentry) acl = state->mbentry->acl;
        /* RFC5464 is a trifle vague about access control for mailbox
         * annotations but this seems to be compliant */
        needed = ACL_LOOKUP|ACL_READ;
        /* fall through to ACL check */
    }
    else if (state->which == ANNOTATION_SCOPE_MESSAGE) {
        assert(state->mailbox);
        acl = state->mailbox->acl;
        /* RFC5257: reading from a private annotation needs 'r'.
         * Reading from a shared annotation needs 'r' */
        needed = ACL_READ;
        /* fall through to ACL check */
    }

    if (!acl)
        return 0;

    my_rights = cyrus_acl_myrights(state->auth_state, acl);

    return ((my_rights & needed) == needed);
}