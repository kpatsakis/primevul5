rlgets(char *s, size_t n, const char *prompt)
{
    static char *line = (char *) NULL;
    static int leftover = -1;	/* index of 1st char leftover from last call */

    if (leftover == -1) {
	/* If we already have a line, first free it */
	if (line != (char *) NULL) {
	    free(line);
	    line = NULL;
	    /* so that ^C or int_error during readline() does
	     * not result in line being free-ed twice */
	}
	line = readline((interactive) ? prompt : "");
	leftover = 0;
	/* If it's not an EOF */
	if (line && *line) {
#  if defined(READLINE) || defined(HAVE_LIBREADLINE)
	    int found;
	    /* Initialize readline history functions */
	    using_history();

	    /* search in the history for entries containing line.
	     * They may have other tokens before and after line, hence
	     * the check on strcmp below. */
	    if (!is_history_command(line)) {
		if (!history_full) {
		    found = history_search(line, -1);
		    if (found != -1 && !strcmp(current_history()->line,line)) {
			/* this line is already in the history, remove the earlier entry */
			HIST_ENTRY *removed = remove_history(where_history());
			/* according to history docs we are supposed to free the stuff */
			if (removed) {
			    free(removed->line);
			    free(removed->data);
			    free(removed);
			}
		    }
		}
		add_history(line);
	    }
#  elif defined(HAVE_LIBEDITLINE)
	    if (!is_history_command(line)) {
		/* deleting history entries does not work, so suppress adjacent duplicates only */
		int found = 0;
		using_history();

		if (!history_full)
		    found = history_search(line, -1);
		if (found <= 0)
		    add_history(line);
	    }
#  endif
	}
    }
    if (line) {
	/* s will be NUL-terminated here */
	safe_strncpy(s, line + leftover, n);
	leftover += strlen(s);
	if (line[leftover] == NUL)
	    leftover = -1;
	return s;
    }
    return NULL;
}