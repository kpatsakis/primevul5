static int annotate_state_need_mbentry(annotate_state_t *state)
{
    int r = 0;

    if (!state->mbentry && state->mailbox) {
        r = mboxlist_lookup(state->mailbox->name, &state->ourmbentry, NULL);
        if (r) {
            syslog(LOG_ERR, "Failed to lookup mbentry for %s: %s",
                    state->mailbox->name, error_message(r));
            goto out;
        }
        state->mbentry = state->ourmbentry;
    }

out:
    return r;
}