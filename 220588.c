ex_tabnext(exarg_T *eap)
{
    int tab_number;

    if (ERROR_IF_POPUP_WINDOW)
	return;
    switch (eap->cmdidx)
    {
	case CMD_tabfirst:
	case CMD_tabrewind:
	    goto_tabpage(1);
	    break;
	case CMD_tablast:
	    goto_tabpage(9999);
	    break;
	case CMD_tabprevious:
	case CMD_tabNext:
	    if (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_number = getdigits(&p);
		if (p == p_save || *p_save == '-' || *p != NUL
			    || tab_number == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invarg2, eap->arg);
		    return;
		}
	    }
	    else
	    {
		if (eap->addr_count == 0)
		    tab_number = 1;
		else
		{
		    tab_number = eap->line2;
		    if (tab_number < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			return;
		    }
		}
	    }
	    goto_tabpage(-tab_number);
	    break;
	default: // CMD_tabnext
	    tab_number = get_tabpage_arg(eap);
	    if (eap->errmsg == NULL)
		goto_tabpage(tab_number);
	    break;
    }
}