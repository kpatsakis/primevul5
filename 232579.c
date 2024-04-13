do_doautocmd(
    char_u	*arg,
    int		do_msg,	    /* give message for no matching autocmds? */
    int		*did_something)
{
    char_u	*fname;
    int		nothing_done = TRUE;
    int		group;

    if (did_something != NULL)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    if (arg == NULL)	    /* out of memory */
	return FAIL;

    if (*arg == '*')
    {
	EMSG(_("E217: Can't execute autocommands for ALL events"));
	return FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    if (fname == NULL)
	return FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    while (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	if (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, NULL, TRUE, group, curbuf, NULL))
	    nothing_done = FALSE;

    if (nothing_done && do_msg)
	MSG(_("No matching autocommands"));
    if (did_something != NULL)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    return aborting() ? FAIL : OK;
#else
    return OK;
#endif
}