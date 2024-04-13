help_command()
{
    int first = c_token;

    while (!END_OF_COMMAND)
	++c_token;

    strcpy(Help, "GNUPLOT ");
    capture(Help + 8, first, c_token - 1, sizeof(Help) - 9);
    help_desc.dsc$w_length = strlen(Help);
    if ((vaxc$errno = lbr$output_help(lib$put_output, 0, &help_desc,
				      &helpfile_desc, 0, lib$get_input)) != SS$_NORMAL)
	os_error(NO_CARET, "can't open GNUPLOT$HELP");
}