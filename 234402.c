save_command()
{
    FILE *fp;
    char *save_file = NULL;
    TBOOLEAN append = FALSE;
    int what;

    c_token++;
    what = lookup_table(&save_tbl[0], c_token);

    switch (what) {
	case SAVE_FUNCS:
	case SAVE_SET:
	case SAVE_TERMINAL:
	case SAVE_VARS:
	case SAVE_FIT:
	case SAVE_DATABLOCKS:
	    c_token++;
	    break;
	default:
	    break;
    }

    save_file = try_to_get_string();
    if (!save_file)
	int_error(c_token, "expecting filename");
    if (equals(c_token, "append")) {
	append = TRUE;
	c_token++;
    }
#ifdef PIPES
    if (save_file[0]=='|') {
	restrict_popen();
	fp = popen(save_file+1,"w");
    } else
#endif
    {
    gp_expand_tilde(&save_file);
#ifdef _WIN32
    fp = !strcmp(save_file,"-") ? stdout
	: loadpath_fopen(save_file, append?"a":"w");
#else
    fp = !strcmp(save_file,"-") ? stdout
	: fopen(save_file, append?"a":"w");
#endif
    }

    if (!fp)
	os_error(c_token, "Cannot open save file");

    switch (what) {
    case SAVE_FUNCS:
	    save_functions(fp);
	break;
    case SAVE_SET:
	    save_set(fp);
	break;
    case SAVE_TERMINAL:
	    save_term(fp);
	break;
    case SAVE_VARS:
	    save_variables(fp);
	break;
    case SAVE_FIT:
	    save_fit(fp);
	break;
    case SAVE_DATABLOCKS:
	    save_datablocks(fp);
	break;
    default:
	    save_all(fp);
    }

    if (stdout != fp) {
#ifdef PIPES
	if (save_file[0] == '|')
	    (void) pclose(fp);
	else
#endif
	    (void) fclose(fp);
    }

    free(save_file);
}