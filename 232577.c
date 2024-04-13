getnextac(int c UNUSED, void *cookie, int indent UNUSED)
{
    AutoPatCmd	    *acp = (AutoPatCmd *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    /* Can be called again after returning the last line. */
    if (acp->curpat == NULL)
	return NULL;

    /* repeat until we find an autocommand to execute */
    for (;;)
    {
	/* skip removed commands */
	while (acp->nextcmd != NULL && acp->nextcmd->cmd == NULL)
	    if (acp->nextcmd->last)
		acp->nextcmd = NULL;
	    else
		acp->nextcmd = acp->nextcmd->next;

	if (acp->nextcmd != NULL)
	    break;

	/* at end of commands, find next pattern that matches */
	if (acp->curpat->last)
	    acp->curpat = NULL;
	else
	    acp->curpat = acp->curpat->next;
	if (acp->curpat != NULL)
	    auto_next_pat(acp, TRUE);
	if (acp->curpat == NULL)
	    return NULL;
    }

    ac = acp->nextcmd;

    if (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg((char_u *)_("autocommand %s"), ac->cmd);
	msg_puts((char_u *)"\n");   /* don't overwrite this either */
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    autocmd_nested = ac->nested;
#ifdef FEAT_EVAL
    current_SID = ac->scriptID;
#endif
    if (ac->last)
	acp->nextcmd = NULL;
    else
	acp->nextcmd = ac->next;
    return retval;
}