find_end_event(
    char_u  *arg,
    int	    have_group)	    /* TRUE when group name was found */
{
    char_u  *pat;
    char_u  *p;

    if (*arg == '*')
    {
	if (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    EMSG2(_("E215: Illegal character after *: %s"), arg);
	    return NULL;
	}
	pat = arg + 1;
    }
    else
    {
	for (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    if ((int)event_name2nr(pat, &p) >= (int)NUM_EVENTS)
	    {
		if (have_group)
		    EMSG2(_("E216: No such event: %s"), pat);
		else
		    EMSG2(_("E216: No such group or event: %s"), pat);
		return NULL;
	    }
	}
    }
    return pat;
}