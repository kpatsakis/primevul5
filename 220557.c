ex_later(exarg_T *eap)
{
    long	count = 0;
    int		sec = FALSE;
    int		file = FALSE;
    char_u	*p = eap->arg;

    if (*p == NUL)
	count = 1;
    else if (isdigit(*p))
    {
	count = getdigits(&p);
	switch (*p)
	{
	    case 's': ++p; sec = TRUE; break;
	    case 'm': ++p; sec = TRUE; count *= 60; break;
	    case 'h': ++p; sec = TRUE; count *= 60 * 60; break;
	    case 'd': ++p; sec = TRUE; count *= 24 * 60 * 60; break;
	    case 'f': ++p; file = TRUE; break;
	}
    }

    if (*p != NUL)
	semsg(_(e_invarg2), eap->arg);
    else
	undo_time(eap->cmdidx == CMD_earlier ? -count : count,
							    sec, file, FALSE);
}