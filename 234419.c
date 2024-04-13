read_line(const char *prompt, int start)
{
    TBOOLEAN more = FALSE;
    int last = 0;

    current_prompt = prompt;

    /* Once we start to read a new line, the tokens pointing into the old */
    /* line are no longer valid.  We used to _not_ clear things here, but */
    /* that lead to errors when a mouse-triggered replot request came in  */
    /* while a new line was being read.   Bug 3602388 Feb 2013.           */
    if (start == 0) {
	c_token = num_tokens = 0;
	gp_input_line[0] = '\0';
    }

    do {
	/* grab some input */
	if (gp_get_string(gp_input_line + start, gp_input_line_len - start,
		         ((more) ? ">" : prompt))
	    == (char *) NULL)
	{
	    /* end-of-file */
	    if (interactive)
		(void) putc('\n', stderr);
	    gp_input_line[start] = NUL;
	    inline_num++;
	    if (start > 0 && curly_brace_count == 0)	/* don't quit yet - process what we have */
		more = FALSE;
	    else
		return (1);	/* exit gnuplot */
	} else {
	    /* normal line input */
	    /* gp_input_line must be NUL-terminated for strlen not to pass the
	     * the bounds of this array */
	    last = strlen(gp_input_line) - 1;
	    if (last >= 0) {
		if (gp_input_line[last] == '\n') {	/* remove any newline */
		    gp_input_line[last] = NUL;
		    if (last > 0 && gp_input_line[last-1] == '\r')
			gp_input_line[--last] = NUL;
		    /* Watch out that we don't backup beyond 0 (1-1-1) */
		    if (last > 0)
			--last;
		} else if (last + 2 >= gp_input_line_len) {
		    extend_input_line();
		    /* read rest of line, don't print "> " */
		    start = last + 1;
		    more = TRUE;
		    continue;
		    /* else fall through to continuation handling */
		} /* if(grow buffer?) */
		if (gp_input_line[last] == '\\') {
		    /* line continuation */
		    start = last;
		    more = TRUE;
		} else
		    more = FALSE;
	    } else
		more = FALSE;
	}
    } while (more);
    return (0);
}