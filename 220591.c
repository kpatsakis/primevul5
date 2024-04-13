ex_redir(exarg_T *eap)
{
    char	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    if (redir_execute)
    {
	emsg(_("E930: Cannot use :redir inside execute()"));
	return;
    }
#endif

    if (STRICMP(eap->arg, "END") == 0)
	close_redir();
    else
    {
	if (*arg == '>')
	{
	    ++arg;
	    if (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    else
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    if (fname == NULL)
		return;
#ifdef FEAT_BROWSE
	    if (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, NULL, NULL,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		if (browseFile == NULL)
		    return;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	else if (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    if (ASCII_ISALPHA(*arg)
# ifdef FEAT_CLIPBOARD
		    || *arg == '*'
		    || *arg == '+'
# endif
		    || *arg == '"')
	    {
		redir_reg = *arg++;
		if (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		else
		{
		    // Can use both "@a" and "@a>".
		    if (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    if (*arg == NUL && !isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    if (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invarg2), eap->arg);
	    }
	}
	else if (*arg == '=' && arg[1] == '>')
	{
	    int append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    if (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    else
		append = FALSE;

	    if (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	else
	    semsg(_(e_invarg2), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    if (redir_fd != NULL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}