ex_buffer(exarg_T *eap)
{
    if (ERROR_IF_ANY_POPUP_WINDOW)
	return;
    if (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_arg, eap->arg);
    else
    {
	if (eap->addr_count == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	else
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (int)eap->line2);
	if (eap->do_ecmd_cmd != NULL)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}