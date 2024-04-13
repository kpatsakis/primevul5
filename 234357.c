do_line()
{
    /* Line continuation has already been handled by read_line() */
    char *inlptr;

    /* Expand any string variables in the current input line */
    string_expand_macros();

    /* Skip leading whitespace */
    inlptr = gp_input_line;
    while (isspace((unsigned char) *inlptr))
	inlptr++;

    /* Leading '!' indicates a shell command that bypasses normal gnuplot
     * tokenization and parsing.  This doesn't work inside a bracketed clause.
     */
    if (is_system(*inlptr)) {
	do_system(inlptr + 1);
	return (0);
    }

    /* Strip off trailing comment */
    FPRINTF((stderr,"doline( \"%s\" )\n", gp_input_line));
    if (strchr(inlptr, '#')) {
	num_tokens = scanner(&gp_input_line, &gp_input_line_len);
	if (gp_input_line[token[num_tokens].start_index] == '#')
	    gp_input_line[token[num_tokens].start_index] = NUL;
    }

    if (inlptr != gp_input_line) {
	/* If there was leading whitespace, copy the actual
	 * command string to the front. use memmove() because
	 * source and target may overlap */
	memmove(gp_input_line, inlptr, strlen(inlptr));
	/* Terminate resulting string */
	gp_input_line[strlen(inlptr)] = NUL;
    }
    FPRINTF((stderr, "  echo: \"%s\"\n", gp_input_line));

    num_tokens = scanner(&gp_input_line, &gp_input_line_len);

    /*
     * Expand line if necessary to contain a complete bracketed clause {...}
     * Insert a ';' after current line and append the next input line.
     * NB: This may leave an "else" condition on the next line.
     */
    if (curly_brace_count < 0)
	int_error(NO_CARET,"Unexpected }");

    while (curly_brace_count > 0) {
	if (lf_head && lf_head->depth > 0) {
	    /* This catches the case that we are inside a "load foo" operation
	     * and therefore requesting interactive input is not an option.
	     * FIXME: or is it?
	     */
	    int_error(NO_CARET, "Syntax error: missing block terminator }");
	}
	else if (interactive || noinputfiles) {
	    /* If we are really in interactive mode and there are unterminated blocks,
	     * then we want to display a "more>" prompt to get the rest of the block.
	     * However, there are two more cases that must be dealt here:
	     * One is when commands are piped to gnuplot - on the command line,
	     * the other is when commands are piped to gnuplot which is opened
	     * as a slave process. The test for noinputfiles is for the latter case.
	     * If we didn't have that test here, unterminated blocks sent via a pipe
	     * would trigger the error message in the else branch below. */
	    int retval;
	    strcat(gp_input_line,";");
	    retval = read_line("more> ", strlen(gp_input_line));
	    if (retval)
		int_error(NO_CARET, "Syntax error: missing block terminator }");
	    /* Expand any string variables in the current input line */
	    string_expand_macros();

	    num_tokens = scanner(&gp_input_line, &gp_input_line_len);
	    if (gp_input_line[token[num_tokens].start_index] == '#')
		gp_input_line[token[num_tokens].start_index] = NUL;
	}
	else {
	    /* Non-interactive mode here means that we got a string from -e.
	     * Having curly_brace_count > 0 means that there are at least one
	     * unterminated blocks in the string.
	     * Likely user error, so we die with an error message. */
	    int_error(NO_CARET, "Syntax error: missing block terminator }");
	}
    }

    c_token = 0;
    while (c_token < num_tokens) {
	command();
	if (command_exit_requested) {
	    command_exit_requested = 0;	/* yes this is necessary */
	    return 1;
	}
	if (iteration_early_exit()) {
	    c_token = num_tokens;
	    break;
	}
	if (c_token < num_tokens) {	/* something after command */
	    if (equals(c_token, ";")) {
		c_token++;
	    } else if (equals(c_token, "{")) {
		begin_clause();
	    } else if (equals(c_token, "}")) {
		end_clause();
	    } else
		int_error(c_token, "unexpected or unrecognized token: %s",
		    token_to_string(c_token));
	}
    }

    /* This check allows event handling inside load/eval/while statements */
    check_for_mouse_events();
    return (0);
}