while_command()
{
    int do_start, do_end;
    char *clause;
    int save_token, end_token;
    double exprval;

    c_token++;
    save_token = c_token;
    exprval = real_expression();

    if (!equals(c_token,"{"))
	int_error(c_token,"expecting {while-clause}");
    end_token = find_clause(&do_start, &do_end);

    clause = new_clause(do_start, do_end);
    begin_clause();

    iteration_depth++;
    while (exprval != 0) {
	requested_continue = FALSE;
	do_string(clause);
	if (command_exit_requested)
	    requested_break = TRUE;
	if (requested_break)
	    break;
	c_token = save_token;
	exprval = real_expression();
    };
    iteration_depth--;

    end_clause();
    free(clause);
    c_token = end_token;
    requested_break = FALSE;
    requested_continue = FALSE;
}