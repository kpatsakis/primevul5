set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    int		doautocmd)	/* TRUE for :doauto*, FALSE for :autocmd */
{
    char_u	*p;
    int		group;

    /* check for a group name, skip it if present */
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    if (group == AUGROUP_ERROR)
	return NULL;
    /* If there only is a group name that's what we expand. */
    if (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    /* skip over event name */
    for (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	if (*p == ',')
	    arg = p + 1;
    if (*p == NUL)
    {
	if (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    /* expand event name */
	xp->xp_pattern = arg;
	return NULL;
    }

    /* skip over pattern */
    arg = skipwhite(p);
    while (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    if (*arg)
	return arg;			    /* expand (next) command */

    if (doautocmd)
	xp->xp_context = EXPAND_FILES;	    /* expand file names */
    else
	xp->xp_context = EXPAND_NOTHING;    /* pattern is not expanded */
    return NULL;
}