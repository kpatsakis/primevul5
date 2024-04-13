static void parse_error(struct parse_state *state, const char *err)
{
    if (++state->nerrors < ANNOT_MAX_ERRORS)
    {
        struct buf msg = BUF_INITIALIZER;

        buf_printf(&msg, "%s:%u:%u:error: %s",
                   state->filename, state->lineno,
                   tok_offset(&state->tok), err);
        if (state->context && *state->context)
            buf_printf(&msg, ", at or near '%s'", state->context);
        syslog(LOG_ERR, "%s", buf_cstring(&msg));
        buf_free(&msg);
    }

    state->context = NULL;
}