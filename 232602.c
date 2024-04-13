au_del_group(char_u *name)
{
    int	    i;

    i = au_find_group(name);
    if (i == AUGROUP_ERROR)	/* the group doesn't exist */
	EMSG2(_("E367: No such group: \"%s\""), name);
    else if (i == current_augroup)
	EMSG(_("E936: Cannot delete the current group"));
    else
    {
	event_T	event;
	AutoPat	*ap;
	int	in_use = FALSE;

	for (event = (event_T)0; (int)event < (int)NUM_EVENTS;
					    event = (event_T)((int)event + 1))
	{
	    for (ap = first_autopat[(int)event]; ap != NULL; ap = ap->next)
		if (ap->group == i && ap->pat != NULL)
		{
		    give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		    in_use = TRUE;
		    event = NUM_EVENTS;
		    break;
		}
	}
	vim_free(AUGROUP_NAME(i));
	if (in_use)
	{
	    AUGROUP_NAME(i) = get_deleted_augroup();
	}
	else
	    AUGROUP_NAME(i) = NULL;
    }
}