call_command()
{
    char *save_file = NULL;

    c_token++;
    save_file = try_to_get_string();

    if (!save_file)
	int_error(c_token, "expecting filename");
    gp_expand_tilde(&save_file);

    /* Argument list follows filename */
    load_file(loadpath_fopen(save_file, "r"), save_file, 2);
}