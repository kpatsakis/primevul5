au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = NULL;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = NULL;
    int		group;
    int		retval = FALSE;

    /* Make a copy so that we can change the '#' chars to a NUL. */
    arg_save = vim_strsave(arg);
    if (arg_save == NULL)
	return FALSE;
    p = vim_strchr(arg_save, '#');
    if (p != NULL)
	*p++ = NUL;

    /* First, look for an autocmd group name */
    group = au_find_group(arg_save);
    if (group == AUGROUP_ERROR)
    {
	/* Didn't match a group name, assume the first argument is an event. */
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    else
    {
	if (p == NULL)
	{
	    /* "Group": group name is present and it's recognized */
	    retval = TRUE;
	    goto theend;
	}

	/* Must be "Group#Event" or "Group#Event#pat". */
	event_name = p;
	p = vim_strchr(event_name, '#');
	if (p != NULL)
	    *p++ = NUL;	    /* "Group#Event#pat" */
    }

    pattern = p;	    /* "pattern" is NULL when there is no pattern */

    /* find the index (enum) for the event name */
    event = event_name2nr(event_name, &p);

    /* return FALSE if the event name is not recognized */
    if (event == NUM_EVENTS)
	goto theend;

    /* Find the first autocommand for this event.
     * If there isn't any, return FALSE;
     * If there is one and no pattern given, return TRUE; */
    ap = first_autopat[(int)event];
    if (ap == NULL)
	goto theend;

    /* if pattern is "<buffer>", special handling is needed which uses curbuf */
    /* for pattern "<buffer=N>, fnamecmp() will work fine */
    if (pattern != NULL && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    /* Check if there is an autocommand with the given pattern. */
    for ( ; ap != NULL; ap = ap->next)
	/* only use a pattern when it has not been removed and has commands. */
	/* For buffer-local autocommands, fnamecmp() works fine. */
	if (ap->pat != NULL && ap->cmds != NULL
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == NULL
		|| (buflocal_buf == NULL
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    break;
	}

theend:
    vim_free(arg_save);
    return retval;
}