static char *get_token(struct parse_state *state, const char *extra)
{
    char *token;
    char *p;

    token = tok_next(&state->tok);
    if (!token) {
        parse_error(state, "invalid annotation attributes");
        return NULL;
    }

    /* check the token */
    if (extra == NULL)
        extra = "";
    for (p = token ; *p && (isalnum(*p) || strchr(extra, *p)) ; p++)
        ;
    if (*p) {
        state->context = p;
        parse_error(state, "invalid character");
        return NULL;
    }

    state->context = token;
    return token;
}