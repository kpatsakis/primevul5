do_command()
{
    t_iterator *do_iterator;
    int do_start, do_end;
    int end_token;
    char *clause;

    c_token++;
    do_iterator = check_for_iteration();
    if (forever_iteration(do_iterator)) {
	cleanup_iteration(do_iterator);
	int_error(c_token-2, "unbounded iteration not accepted here");
    }

    if (!equals(c_token,"{")) {
	cleanup_iteration(do_iterator);
	int_error(c_token,"expecting {do-clause}");
    }
    end_token = find_clause(&do_start, &do_end);

    clause = new_clause(do_start, do_end);
    begin_clause();

    iteration_depth++;

    /* Sometimes the start point of a nested iteration is not within the
     * limits for all levels of nesting. In this case we need to advance
     * through the iteration to find the first good set of indices.
     * If we don't find one, forget the whole thing.
     */
    if (empty_iteration(do_iterator) && !next_iteration(do_iterator)) {
	strcpy(clause, ";");
    }

    do {
	requested_continue = FALSE;
	do_string(clause);

	if (command_exit_requested != 0)
	    requested_break = TRUE;
	if (requested_break)
	    break;
    } while (next_iteration(do_iterator));
    iteration_depth--;

    free(clause);
    end_clause();
    c_token = end_token;

    /* FIXME:  If any of the above exited via int_error() then this	*/
    /* cleanup never happens and we leak memory.  But do_iterator can	*/
    /* not be static or global because do_command() can recurse.	*/
    do_iterator = cleanup_iteration(do_iterator);
    requested_break = FALSE;
    requested_continue = FALSE;
}