ex_winsize(exarg_T *eap)
{
    int		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    if (!isdigit(*arg))
    {
	semsg(_(e_invarg2), arg);
	return;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    if (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    else
	emsg(_("E465: :winsize requires two number arguments"));
}