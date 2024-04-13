ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    if (not_in_vim9(eap) == FAIL)
	return;
#endif
    if (*eap->arg == NUL)		// No argument?
	emsg(_(e_argreq));
    else if (eap->arg[1] != NUL)	// more than one character?
	semsg(_(e_trailing_arg), eap->arg);
    else
    {
	pos = curwin->w_cursor;		// save curwin->w_cursor
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_WHITE | BL_FIX);
	if (setmark(*eap->arg) == FAIL)	// set mark
	    emsg(_("E191: Argument must be a letter or forward/backward quote"));
	curwin->w_cursor = pos;		// restore curwin->w_cursor
    }
}