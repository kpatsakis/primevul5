old_if_command(struct at_type *expr)
{
    struct value condition;
    char *if_start, *if_end;
    char *else_start = NULL;

    if (clause_depth > 0)
	int_error(c_token,"Old-style if/else statement encountered inside brackets");

    evaluate_at(expr, &condition);

    /* find start and end of the "if" part */
    if_start = &gp_input_line[ token[c_token].start_index ];
    while ((c_token < num_tokens) && !equals(c_token,"else"))
	c_token++;

    if (equals(c_token,"else")) {
	if_end = &gp_input_line[ token[c_token].start_index - 1 ];
	*if_end = '\0';
	else_start = &gp_input_line[ token[c_token].start_index + token[c_token].length ];
    }

    if (real(&condition) != 0.0)
	do_string(if_start);
    else if (else_start)
	do_string(else_start);

    c_token = num_tokens = 0;		/* discard rest of line */
}