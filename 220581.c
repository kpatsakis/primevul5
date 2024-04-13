ex_read(exarg_T *eap)
{
    int		i;
    int		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    if (eap->usefilter)			// :r!cmd
	do_bang(1, eap, FALSE, FALSE, TRUE);
    else
    {
	if (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	    return;

#ifdef FEAT_BROWSE
	if (cmdmod.cmod_flags & CMOD_BROWSE)
	{
	    char_u *browseFile;

	    browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
						    NULL, NULL, NULL, curbuf);
	    if (browseFile != NULL)
	    {
		i = readfile(browseFile, NULL,
			  eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
		vim_free(browseFile);
	    }
	    else
		i = OK;
	}
	else
#endif
	     if (*eap->arg == NUL)
	{
	    if (check_fname() == FAIL)	// check for no file name
		return;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
			  eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	else
	{
	    if (vim_strchr(p_cpo, CPO_ALTREAD) != NULL)
		(void)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, NULL,
			  eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
	if (i != OK)
	{
#if defined(FEAT_EVAL)
	    if (!aborting())
#endif
		semsg(_(e_notopen), eap->arg);
	}
	else
	{
	    if (empty && exmode_active)
	    {
		// Delete the empty line that remains.  Historically ex does
		// this but vi doesn't.
		if (eap->line2 == 0)
		    lnum = curbuf->b_ml.ml_line_count;
		else
		    lnum = 1;
		if (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
		{
		    ml_delete(lnum);
		    if (curwin->w_cursor.lnum > 1
					     && curwin->w_cursor.lnum >= lnum)
			--curwin->w_cursor.lnum;
		    deleted_lines_mark(lnum, 1L);
		}
	    }
	    redraw_curbuf_later(VALID);
	}
    }
}