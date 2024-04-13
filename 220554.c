ex_findpat(exarg_T *eap)
{
    int		whole = TRUE;
    long	n;
    char_u	*p;
    int		action;

    switch (cmdnames[eap->cmdidx].cmd_name[2])
    {
	case 'e':	// ":psearch", ":isearch" and ":dsearch"
		if (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		else
		    action = ACTION_SHOW;
		break;
	case 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		break;
	case 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		break;
	default:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		break;
    }

    n = 1;
    if (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    if (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	if (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    if (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_arg, p);
	    else
		set_nextcmd(eap, p);
	}
    }
    if (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (int)STRLEN(eap->arg),
			    whole, !eap->forceit,
			    *eap->cmd == 'd' ?	FIND_DEFINE : FIND_ANY,
			    n, action, eap->line1, eap->line2);
}