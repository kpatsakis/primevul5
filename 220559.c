ex_sleep(exarg_T *eap)
{
    int		n;
    long	len;

    if (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - msg_scrolled;
	if (n >= 0)
	    windgoto((int)n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    switch (*eap->arg)
    {
	case 'm': break;
	case NUL: len *= 1000L; break;
	default: semsg(_(e_invarg2), eap->arg); return;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}