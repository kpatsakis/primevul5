do_augroup(char_u *arg, int del_group)
{
    int	    i;

    if (del_group)
    {
	if (*arg == NUL)
	    EMSG(_(e_argreq));
	else
	    au_del_group(arg);
    }
    else if (STRICMP(arg, "end") == 0)   /* ":aug end": back to group 0 */
	current_augroup = AUGROUP_DEFAULT;
    else if (*arg)		    /* ":aug xxx": switch to group xxx */
    {
	i = au_new_group(arg);
	if (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    else			    /* ":aug": list the group names */
    {
	msg_start();
	for (i = 0; i < augroups.ga_len; ++i)
	{
	    if (AUGROUP_NAME(i) != NULL)
	    {
		msg_puts(AUGROUP_NAME(i));
		msg_puts((char_u *)"  ");
	    }
	}
	msg_clr_eos();
	msg_end();
    }
}