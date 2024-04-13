replotrequest()
{
    /* do not store directly into the replot_line string until the
     * new plot line has been successfully plotted. This way,
     * if user makes a typo in a replot line, they do not have
     * to start from scratch. The replot_line will be committed
     * after do_plot has returned, whence we know all is well
     */
    if (END_OF_COMMAND) {
	char *rest_args = &gp_input_line[token[c_token].start_index];
	size_t replot_len = strlen(replot_line);
	size_t rest_len = strlen(rest_args);

	/* preserve commands following 'replot ;' */
	/* move rest of input line to the start
	 * necessary because of realloc() in extend_input_line() */
	memmove(gp_input_line,rest_args,rest_len+1);
	/* reallocs if necessary */
	while (gp_input_line_len < replot_len+rest_len+1)
	    extend_input_line();
	/* move old rest args off begin of input line to
	 * make space for replot_line */
	memmove(gp_input_line+replot_len,gp_input_line,rest_len+1);
	/* copy previous plot command to start of input line */
	memcpy(gp_input_line, replot_line, replot_len);
    } else {
	char *replot_args = NULL;	/* else m_capture will free it */
	int last_token = num_tokens - 1;

	/* length = length of old part + length of new part + ", " + \0 */
	size_t newlen = strlen(replot_line) + token[last_token].start_index
		      + token[last_token].length - token[c_token].start_index + 3;

	m_capture(&replot_args, c_token, last_token);	/* might be empty */
	while (gp_input_line_len < newlen)
	    extend_input_line();
	strcpy(gp_input_line, replot_line);
	strcat(gp_input_line, ", ");
	strcat(gp_input_line, replot_args);
	free(replot_args);
    }
    plot_token = 0;		/* whole line to be saved as replot line */
    SET_REFRESH_OK(E_REFRESH_NOT_OK, 0);		/* start of replot will destroy existing data */

    screen_ok = FALSE;
    num_tokens = scanner(&gp_input_line, &gp_input_line_len);
    c_token = 1;	/* Skip the "plot" token */

    if (almost_equals(0,"test")) {
	c_token = 0;
	test_command();
    } else if (almost_equals(0,"s$plot"))
	plot3drequest();
    else
	plotrequest();
}