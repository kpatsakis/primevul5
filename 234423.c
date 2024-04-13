history_command()
{
#ifdef USE_READLINE
    c_token++;

    if (!END_OF_COMMAND && equals(c_token,"?")) {
	static char *search_str = NULL;  /* string from command line to search for */

	/* find and show the entries */
	c_token++;
	m_capture(&search_str, c_token, c_token);  /* reallocates memory */
	printf ("history ?%s\n", search_str);
	if (!history_find_all(search_str))
	    int_error(c_token,"not in history");
	c_token++;

    } else if (!END_OF_COMMAND && equals(c_token,"!")) {
	const char *line_to_do = NULL;  /* command returned by search	*/

	c_token++;
	if (isanumber(c_token)) {
	    int i = int_expression();
	    line_to_do = history_find_by_number(i);
	} else {
	    char *search_str = NULL;  /* string from command line to search for */
	    m_capture(&search_str, c_token, c_token);
	    line_to_do = history_find(search_str);
	    free(search_str);
	}
	if (line_to_do == NULL)
	    int_error(c_token, "not in history");

	/* Add the command to the history.
	   Note that history commands themselves are no longer added to the history. */
	add_history((char *) line_to_do);

	printf("  Executing:\n\t%s\n", line_to_do);
	do_string(line_to_do);
	c_token++;

    } else {
	int n = 0;		   /* print only <last> entries */
	char *tmp;
	TBOOLEAN append = FALSE;   /* rewrite output file or append it */
	static char *name = NULL;  /* name of the output file; NULL for stdout */

	TBOOLEAN quiet = history_quiet;
	if (!END_OF_COMMAND && almost_equals(c_token,"q$uiet")) {
	    /* option quiet to suppress history entry numbers */
	    quiet = TRUE;
	    c_token++;
	}
	/* show history entries */
	if (!END_OF_COMMAND && isanumber(c_token)) {
	    n = int_expression();
	}
	if ((tmp = try_to_get_string())) {
	    free(name);
	    name = tmp;
	    if (!END_OF_COMMAND && almost_equals(c_token, "ap$pend")) {
		append = TRUE;
		c_token++;
	    }
	}
	write_history_n(n, (quiet ? "" : name), (append ? "a" : "w"));
    }

#else
    c_token++;
    int_warn(NO_CARET, "This copy of gnuplot was built without support for command history.");
#endif /* defined(USE_READLINE) */
}