get_tabpage_arg(exarg_T *eap)
{
    int tab_number;
    int unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    if (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	int    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	if (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	else if (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_number = getdigits(&p);

	if (relative == 0)
	{
	    if (STRCMP(p, "$") == 0)
		tab_number = LAST_TAB_NR;
	    else if (STRCMP(p, "#") == 0)
		if (valid_tabpage(lastused_tabpage))
		    tab_number = tabpage_index(lastused_tabpage);
		else
		{
		    eap->errmsg = ex_errmsg(e_invargval, eap->arg);
		    tab_number = 0;
		    goto theend;
		}
	    else if (p == p_save || *p_save == '-' || *p != NUL
		    || tab_number > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invarg2, eap->arg);
		goto theend;
	    }
	}
	else
	{
	    if (*p_save == NUL)
		tab_number = 1;
	    else if (p == p_save || *p_save == '-' || *p != NUL
		    || tab_number == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invarg2, eap->arg);
		goto theend;
	    }
	    tab_number = tab_number * relative + tabpage_index(curtab);
	    if (!unaccept_arg0 && relative == -1)
		--tab_number;
	}
	if (tab_number < unaccept_arg0 || tab_number > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invarg2, eap->arg);
    }
    else if (eap->addr_count > 0)
    {
	if (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_number = 0;
	}
	else
	{
	    tab_number = eap->line2;
	    if (!unaccept_arg0 && *skipwhite(*eap->cmdlinep) == '-')
	    {
		--tab_number;
		if (tab_number < unaccept_arg0)
		    eap->errmsg = _(e_invalid_range);
	    }
	}
    }
    else
    {
	switch (eap->cmdidx)
	{
	case CMD_tabnext:
	    tab_number = tabpage_index(curtab) + 1;
	    if (tab_number > LAST_TAB_NR)
		tab_number = 1;
	    break;
	case CMD_tabmove:
	    tab_number = LAST_TAB_NR;
	    break;
	default:
	    tab_number = tabpage_index(curtab);
	}
    }

theend:
    return tab_number;
}