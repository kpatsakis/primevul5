static int parse_table_lookup_bitmask(const struct annotate_attrib *table,
                                      struct parse_state *state)
{
    char *token = get_token(state, ".-_/ ");
    char *p;
    int i;
    int result = 0;
    tok_t tok;

    if (!token)
        return -1;
    tok_initm(&tok, token, NULL, 0);

    while ((p = tok_next(&tok))) {
        state->context = p;
        i = table_lookup(table, p);
        if (i < 0)
            return i;
        result |= i;
    }

    return result;
}