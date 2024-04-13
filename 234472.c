set_term()
{
    struct termentry *t = NULL;
    char *input_name = NULL;

    if (!END_OF_COMMAND) {
	input_name = gp_input_line + token[c_token].start_index;
	t = change_term(input_name, token[c_token].length);
	if (!t && isstringvalue(c_token) && (input_name = try_to_get_string())) {
	    if (strchr(input_name, ' '))
		*strchr(input_name, ' ') = '\0';
	    t = change_term(input_name, strlen(input_name));
	    free(input_name);
	} else {
	    c_token++;
	}
    }

    if (!t) {
	change_term("unknown", 7);
	int_error(c_token-1, "unknown or ambiguous terminal type; type just 'set terminal' for a list");
    }

    /* otherwise the type was changed */
    return (t);
}