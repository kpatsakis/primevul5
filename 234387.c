if_else_command(ifstate if_state)
{
    int clause_start, clause_end;
    int next_token;

    /* initial or recursive ("else if") if clause */
    if (equals(c_token,"if")) {
	struct at_type *expr;

	if (!equals(++c_token, "("))
	    int_error(c_token, "expecting (expression)");
	/* advance past if condition whether or not we evaluate it */
	expr = temp_at();
	if (equals(c_token,"{")) {
	    next_token = find_clause(&clause_start, &clause_end);
	} else {
	    /* pre-v5 syntax for "if" with no curly brackets */
	    old_if_command(expr);
	    return;
	}
	if (if_state == IF_TRUE) {
	    /* This means we are here recursively in an "else if"
	     * following an "if" clause that was already executed.
	     * Skip both the expression and the bracketed clause.
	     */
	    c_token = next_token;
	} else if (TRUE || if_state == IF_INITIAL) {
	    struct value condition;
	    evaluate_at(expr, &condition);
	    if (real(&condition) == 0) {
		if_state = IF_FALSE;
		c_token = next_token;
	    } else  {
		char *clause;
		if_state = IF_TRUE;
		clause = new_clause(clause_start, clause_end);
		begin_clause();
		do_string_and_free(clause);
		end_clause();
		if (iteration_early_exit())
		    c_token = num_tokens;
		else
		    c_token = next_token;
	    }
	} else {
	    int_error(c_token, "unexpected if_state");
	}
    }

    /* Done with "if" portion.  Check for "else" */
    if (equals(c_token,"else")) {
	c_token++;
	if (equals(c_token,"if")) {
	    if_else_command(if_state);
	} else if (equals(c_token,"{")) {
	    next_token = find_clause(&clause_start, &clause_end);
	    if (if_state == IF_TRUE) {
		c_token = next_token;
	    } else {
		char *clause;
		if_state = IF_TRUE;
		clause = new_clause(clause_start, clause_end);
		begin_clause();
		do_string_and_free(clause);
		end_clause();
		if (iteration_early_exit())
		    c_token = num_tokens;
		else
		    c_token = next_token;
	    }
	    if_open_for_else = FALSE;
	} else {
	    int_error(c_token, "expecting bracketed else clause");
	}
    } else {
	/* There was no "else" on this line but we might see it on another line */
	if_open_for_else = !(if_state == IF_TRUE);
    }

    return;
}