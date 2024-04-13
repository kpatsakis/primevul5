au_cleanup(void)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    if (autocmd_busy || !au_need_clean)
	return;

    /* loop over all events */
    for (event = (event_T)0; (int)event < (int)NUM_EVENTS;
					    event = (event_T)((int)event + 1))
    {
	/* loop over all autocommand patterns */
	prev_ap = &(first_autopat[(int)event]);
	for (ap = *prev_ap; ap != NULL; ap = *prev_ap)
	{
	    /* loop over all commands for this pattern */
	    prev_ac = &(ap->cmds);
	    for (ac = *prev_ac; ac != NULL; ac = *prev_ac)
	    {
		/* remove the command if the pattern is to be deleted or when
		 * the command has been marked for deletion */
		if (ap->pat == NULL || ac->cmd == NULL)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		else
		    prev_ac = &(ac->next);
	    }

	    /* remove the pattern if it has been marked for deletion */
	    if (ap->pat == NULL)
	    {
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    else
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}