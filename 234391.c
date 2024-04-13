exit_command()
{
    /* If the command is "exit gnuplot" then do so */
    if (equals(c_token+1,"gnuplot"))
	gp_exit(EXIT_SUCCESS);

    if (equals(c_token+1,"status")) {
	int status;
	c_token += 2;
	status = int_expression();
	gp_exit(status);
    }

    /* exit error 'error message'  returns to the top command line */
    if (equals(c_token+1,"error")) {
	c_token += 2;
	int_error(NO_CARET, try_to_get_string());
    }

    /* else graphics will be tidied up in main */
    command_exit_requested = 1;
}