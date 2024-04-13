extend_token_table()
{
    if (token_table_size == 0) {
	/* first time */
	token = (struct lexical_unit *) gp_alloc(MAX_TOKENS * sizeof(struct lexical_unit), "token table");
	token_table_size = MAX_TOKENS;
	/* HBB: for checker-runs: */
	memset(token, 0, MAX_TOKENS * sizeof(*token));
    } else {
	token = gp_realloc(token, (token_table_size + MAX_TOKENS) * sizeof(struct lexical_unit), "extend token table");
	memset(token+token_table_size, 0, MAX_TOKENS * sizeof(*token));
	token_table_size += MAX_TOKENS;
	FPRINTF((stderr, "extending token table to %d elements\n", token_table_size));
    }
}