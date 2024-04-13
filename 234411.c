load_command()
{

    c_token++;
    if (equals(c_token, "$") && isletter(c_token+1) && !equals(c_token+2,"[")) {
	/* "load" a datablock rather than a file */
	/* datablock_name will eventually be freed by lf_pop() */
	char *datablock_name = gp_strdup(parse_datablock_name());
	load_file(NULL, datablock_name, 6);
    } else {
	/* These need to be local so that recursion works. */
	/* They will eventually be freed by lf_pop(). */
	FILE *fp;
	char *save_file = try_to_get_string();
	if (!save_file)
	    int_error(c_token, "expecting filename");
	gp_expand_tilde(&save_file);
	fp = strcmp(save_file, "-") ? loadpath_fopen(save_file, "r") : stdout;
	load_file(fp, save_file, 1);
    }
}