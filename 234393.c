bind_command()
{
    char* lhs = NULL;
    char* rhs = NULL;
    TBOOLEAN allwindows = FALSE;
    ++c_token;

    if (almost_equals(c_token,"all$windows")) {
	allwindows = TRUE;
	c_token++;
    }

    /* get left hand side: the key or key sequence
     * either (1) entire sequence is in quotes
     * or (2) sequence goes until the first whitespace
     */
    if (END_OF_COMMAND) {
	; /* Fall through */
    } else if (isstringvalue(c_token) && (lhs = try_to_get_string())) {
	FPRINTF((stderr,"Got bind quoted lhs = \"%s\"\n",lhs));
    } else {
	char *first = gp_input_line + token[c_token].start_index;
	int size = strcspn(first, " \";");
	lhs = gp_alloc(size + 1, "bind_command->lhs");
	strncpy(lhs, first, size);
	lhs[size] = '\0';
	FPRINTF((stderr,"Got bind unquoted lhs = \"%s\"\n",lhs));
	while (gp_input_line + token[c_token].start_index < first+size)
	    c_token++;
    }

    /* get right hand side: the command to bind
     * either (1) quoted command
     * or (2) the rest of the line
     */
    if (END_OF_COMMAND) {
	; /* Fall through */
    } else if (isstringvalue(c_token) && (rhs = try_to_get_string())) {
	FPRINTF((stderr,"Got bind quoted rhs = \"%s\"\n",rhs));
    } else {
	int save_token = c_token;
	while (!END_OF_COMMAND)
	    c_token++;
	m_capture( &rhs, save_token, c_token-1 );
	FPRINTF((stderr,"Got bind unquoted rhs = \"%s\"\n",rhs));
    }

    /* bind_process() will eventually free lhs / rhs ! */
    bind_process(lhs, rhs, allwindows);

}