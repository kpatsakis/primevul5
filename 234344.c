changedir_command()
{
    char *save_file = NULL;

    c_token++;
    save_file = try_to_get_string();
    if (!save_file)
	int_error(c_token, "expecting directory name");

    gp_expand_tilde(&save_file);
    if (changedir(save_file))
	int_error(c_token, "Can't change to this directory");
    else
	update_gpval_variables(5);
    free(save_file);
}