eval_command()
{
    char *command;
    c_token++;
    command = try_to_get_string();
    if (!command)
	int_error(c_token, "Expected command string");
    do_string_and_free(command);
}