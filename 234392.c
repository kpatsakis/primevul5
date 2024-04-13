print_command()
{
    struct value a;
    /* space printed between two expressions only */
    TBOOLEAN need_space = FALSE;
    char *dataline = NULL;
    size_t size = 256;
    size_t len = 0;

    if (!print_out)
	print_out = stderr;
    if (print_out_var != NULL) { /* print to datablock */
	dataline = (char *) gp_alloc(size, "dataline");
	*dataline = NUL;
    }
    screen_ok = FALSE;
    do {
	++c_token;
	if (equals(c_token, "$") && isletter(c_token+1) && !equals(c_token+2,"[")) {
	    char *datablock_name = parse_datablock_name();
	    char **line = get_datablock(datablock_name);

	    /* Printing a datablock into itself would cause infinite recursion */
	    if (print_out_var && !strcmp(datablock_name, print_out_name))
		continue;

	    while (line && *line) {
		if (print_out_var != NULL)
		    append_to_datablock(&print_out_var->udv_value, strdup(*line));
		else
		    fprintf(print_out, "%s\n", *line);
		line++;
	    }
	    continue;
	}
	if (type_udv(c_token) == ARRAY && !equals(c_token+1, "[")) {
	    udvt_entry *array = add_udv(c_token++);
	    save_array_content(print_out, array->udv_value.v.value_array);
	    continue;
	}
	const_express(&a);
	if (a.type == STRING) {
	    if (dataline != NULL)
		len = strappend(&dataline, &size, len, a.v.string_val);
	    else
		fputs(a.v.string_val, print_out);
	    need_space = FALSE;
	} else {
	    if (need_space) {
		if (dataline != NULL)
		    len = strappend(&dataline, &size, len, " ");
		else
		    putc(' ', print_out);
	    }
	    if (dataline != NULL)
		len = strappend(&dataline, &size, len, value_to_str(&a, FALSE));
	    else
		disp_value(print_out, &a, FALSE);
	    need_space = TRUE;
	}
	free_value(&a);

    } while (!END_OF_COMMAND && equals(c_token, ","));

    if (dataline != NULL) {
	append_multiline_to_datablock(&print_out_var->udv_value, dataline);
    } else {
	(void) putc('\n', print_out);
	fflush(print_out);
    }
}