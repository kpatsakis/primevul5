EXPORTED void annotate_state_set_auth(annotate_state_t *state,
                             int isadmin, const char *userid,
                             const struct auth_state *auth_state)
{
    /* Note: lmtpd sometimes calls through the append code with
     * auth_state=NULL, so we cannot rely on it being non-NULL */
    state->userid = userid;
    state->isadmin = isadmin;
    state->auth_state = auth_state;
}