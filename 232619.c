msg_add_lines(
    int	    insert_space,
    long    lnum,
    off_T   nchars)
{
    char_u  *p;

    p = IObuff + STRLEN(IObuff);

    if (insert_space)
	*p++ = ' ';
    if (shortmess(SHM_LINES))
	vim_snprintf((char *)p, IOSIZE - (p - IObuff),
		"%ldL, %lldC", lnum, (varnumber_T)nchars);
    else
    {
	if (lnum == 1)
	    STRCPY(p, _("1 line, "));
	else
	    sprintf((char *)p, _("%ld lines, "), lnum);
	p += STRLEN(p);
	if (nchars == 1)
	    STRCPY(p, _("1 character"));
	else
	    vim_snprintf((char *)p, IOSIZE - (p - IObuff),
		    _("%lld characters"), (varnumber_T)nchars);
    }
}