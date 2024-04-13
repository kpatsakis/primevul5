show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    /* Check for "got_int" (here and at various places below), which is set
     * when "q" has been hit for the "--more--" prompt */
    if (got_int)
	return;
    if (ap->pat == NULL)		/* pattern has been removed */
	return;

    msg_putchar('\n');
    if (got_int)
	return;
    if (event != last_event || ap->group != last_group)
    {
	if (ap->group != AUGROUP_DEFAULT)
	{
	    if (AUGROUP_NAME(ap->group) == NULL)
		msg_puts_attr(get_deleted_augroup(), HL_ATTR(HLF_E));
	    else
		msg_puts_attr(AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    msg_puts((char_u *)"  ");
	}
	msg_puts_attr(event_nr2name(event), HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	msg_putchar('\n');
	if (got_int)
	    return;
    }
    msg_col = 4;
    msg_outtrans(ap->pat);

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	if (ac->cmd != NULL)		/* skip removed commands */
	{
	    if (msg_col >= 14)
		msg_putchar('\n');
	    msg_col = 14;
	    if (got_int)
		return;
	    msg_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	    if (p_verbose > 0)
		last_set_msg(ac->scriptID);
#endif
	    if (got_int)
		return;
	    if (ac->next != NULL)
	    {
		msg_putchar('\n');
		if (got_int)
		    return;
	    }
	}
    }
}