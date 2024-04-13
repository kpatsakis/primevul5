skip_range(
    char_u	*cmd,
    int		skip_star,	// skip "*" used for Visual range
    int		*ctx)		// pointer to xp_context or NULL
{
    unsigned	delim;

    while (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != NULL)
    {
	if (*cmd == '\\')
	{
	    if (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    else
		break;
	}
	else if (*cmd == '\'')
	{
	    if (*++cmd == NUL && ctx != NULL)
		*ctx = EXPAND_NOTHING;
	}
	else if (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    while (*cmd != NUL && *cmd != delim)
		if (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    if (*cmd == NUL && ctx != NULL)
		*ctx = EXPAND_NOTHING;
	}
	if (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    while (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    if (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == NULL)
	cmd = skipwhite(cmd + 1);

    return cmd;
}