buf_reload(buf_T *buf, int orig_mode)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    int		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    int		saved = OK;
    aco_save_T	aco;
    int		flags = READ_NEW;

    /* set curwin/curbuf for "buf" and save some things */
    aucmd_prepbuf(&aco, buf);

    /* We only want to read the text from the file, not reset the syntax
     * highlighting, clear marks, diff status, etc.  Force the fileformat
     * and encoding to be the same. */
    if (prep_exarg(&ea, buf) == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	if (p_ur < 0 || curbuf->b_ml.ml_line_count <= p_ur)
	{
	    /* Save all the text, so that the reload can be undone.
	     * Sync first so that this is a separate undo-able action. */
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_line_count + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	if (BUFEMPTY() || saved == FAIL)
	    savebuf = NULL;
	else
	{
	    /* Allocate a buffer without putting it in the buffer list. */
	    savebuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    if (savebuf != NULL && buf == curbuf)
	    {
		/* Open the memline. */
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    if (savebuf == NULL || saved == FAIL || buf != curbuf
				      || move_lines(buf, savebuf) == FAIL)
	    {
		EMSG2(_("E462: Could not prepare for reloading \"%s\""),
							    buf->b_fname);
		saved = FAIL;
	    }
	}

	if (saved == OK)
	{
	    curbuf->b_flags |= BF_CHECK_RO;	/* check for RO again */
#ifdef FEAT_AUTOCMD
	    keep_filetype = TRUE;		/* don't detect 'filetype' */
#endif
	    if (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#if defined(FEAT_AUTOCMD) && defined(FEAT_EVAL)
		if (!aborting())
#endif
		    EMSG2(_("E321: Could not reload \"%s\""), buf->b_fname);
		if (savebuf != NULL && bufref_valid(&bufref) && buf == curbuf)
		{
		    /* Put the text back from the save buffer.  First
		     * delete any lines that readfile() added. */
		    while (!BUFEMPTY())
			if (ml_delete(buf->b_ml.ml_line_count, FALSE) == FAIL)
			    break;
		    (void)move_lines(savebuf, buf);
		}
	    }
	    else if (buf == curbuf)  /* "buf" still valid */
	    {
		/* Mark the buffer as unmodified and free undo info. */
		unchanged(buf, TRUE);
		if ((flags & READ_KEEP_UNDO) == 0)
		{
		    u_blockfree(buf);
		    u_clearall(buf);
		}
		else
		{
		    /* Mark all undo states as changed. */
		    u_unchanged(curbuf);
		}
	    }
	}
	vim_free(ea.cmd);

	if (savebuf != NULL && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	/* Invalidate diff info if necessary. */
	diff_invalidate(curbuf);
#endif

	/* Restore the topline and cursor position and check it (lines may
	 * have been removed). */
	if (old_topline > curbuf->b_ml.ml_line_count)
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
	else
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
#ifdef FEAT_AUTOCMD
	keep_filetype = FALSE;
#endif
#ifdef FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    /* Update folds unless they are defined manually. */
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		if (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	/* If the mode didn't change and 'readonly' was set, keep the old
	 * value; the user probably used the ":view" command.  But don't
	 * reset it, might have had a read error. */
	if (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	/* Modelines must override settings done by autocommands. */
	do_modelines(0);
    }

    /* restore curwin/curbuf and a few other things */
    aucmd_restbuf(&aco);
    /* Careful: autocommands may have made "buf" invalid! */
}