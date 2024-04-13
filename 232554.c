auto_next_pat(
    AutoPatCmd	*apc,
    int		stop_at_last)	    /* stop when 'last' flag is set */
{
    AutoPat	*ap;
    AutoCmd	*cp;
    char_u	*name;
    char	*s;

    vim_free(sourcing_name);
    sourcing_name = NULL;

    for (ap = apc->curpat; ap != NULL && !got_int; ap = ap->next)
    {
	apc->curpat = NULL;

	/* Only use a pattern when it has not been removed, has commands and
	 * the group matches. For buffer-local autocommands only check the
	 * buffer number. */
	if (ap->pat != NULL && ap->cmds != NULL
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    /* execution-condition */
	    if (ap->buflocal_nr == 0
		    ? (match_file_pat(NULL, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		name = event_nr2name(apc->event);
		s = _("%s Auto commands for \"%s\"");
		sourcing_name = alloc((unsigned)(STRLEN(s)
					    + STRLEN(name) + ap->patlen + 1));
		if (sourcing_name != NULL)
		{
		    sprintf((char *)sourcing_name, s,
					       (char *)name, (char *)ap->pat);
		    if (p_verbose >= 8)
		    {
			verbose_enter();
			smsg((char_u *)_("Executing %s"), sourcing_name);
			verbose_leave();
		    }
		}

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		/* mark last command */
		for (cp = ap->cmds; cp->next != NULL; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    line_breakcheck();
	    if (apc->curpat != NULL)	    /* found a match */
		break;
	}
	if (stop_at_last && ap->last)
	    break;
    }
}