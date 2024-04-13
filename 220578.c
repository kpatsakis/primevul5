ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    int		tab_number;

# ifdef FEAT_CMDWIN
    if (cmdwin_type != 0)
	cmdwin_result = K_IGNORE;
    else
# endif
	if (first_tabpage->tp_next == NULL)
	    emsg(_("E784: Cannot close last tab page"));
	else
	{
	    tab_number = get_tabpage_arg(eap);
	    if (eap->errmsg == NULL)
	    {
		tp = find_tabpage(tab_number);
		if (tp == NULL)
		{
		    beep_flush();
		    return;
		}
		if (tp != curtab)
		{
		    tabpage_close_other(tp, eap->forceit);
		    return;
		}
		else if (!text_locked() && !curbuf_locked())
		    tabpage_close(eap->forceit);
	    }
	}
}