aucmd_restbuf(
    aco_save_T	*aco)		/* structure holding saved values */
{
    int dummy;

    if (aco->use_aucmd_win)
    {
	--curbuf->b_nwindows;
	/* Find "aucmd_win", it can't be closed, but it may be in another tab
	 * page. Do not trigger autocommands here. */
	block_autocmds();
	if (curwin != aucmd_win)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		if (wp == aucmd_win)
		{
		    if (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(aucmd_win);
		    goto win_found;
		}
	    }
	}
win_found:

	/* Remove the window and frame from the tree of frames. */
	(void)winframe_remove(curwin, &dummy, NULL);
	win_remove(curwin, NULL);
	aucmd_win_used = FALSE;
	last_status(FALSE);	    /* may need to remove last status line */

	if (!valid_tabpage_win(curtab))
	    /* no valid window in current tabpage */
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	(void)win_comp_pos();   /* recompute window positions */
	unblock_autocmds();

	if (win_valid(aco->save_curwin))
	    curwin = aco->save_curwin;
	else
	    /* Hmm, original window disappeared.  Just use the first one. */
	    curwin = firstwin;
#ifdef FEAT_EVAL
	vars_clear(&aucmd_win->w_vars->dv_hashtab);  /* free all w: variables */
	hash_init(&aucmd_win->w_vars->dv_hashtab);   /* re-use the hashtab */
#endif
	curbuf = curwin->w_buffer;

	vim_free(globaldir);
	globaldir = aco->globaldir;

	/* the buffer contents may have changed */
	check_cursor();
	if (curwin->w_topline > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#if defined(FEAT_GUI)
	/* Hide the scrollbars from the aucmd_win and update. */
	gui_mch_enable_scrollbar(&aucmd_win->w_scrollbars[SBAR_LEFT], FALSE);
	gui_mch_enable_scrollbar(&aucmd_win->w_scrollbars[SBAR_RIGHT], FALSE);
	gui_may_update_scrollbars();
#endif
    }
    else
    {
	/* restore curwin */
	if (win_valid(aco->save_curwin))
	{
	    /* Restore the buffer which was previously edited by curwin, if
	     * it was changed, we are still the same window and the buffer is
	     * valid. */
	    if (curwin == aco->new_curwin
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != NULL)
	    {
# if defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		if (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
# endif
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_buffer = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = aco->save_curwin;
	    curbuf = curwin->w_buffer;
	    /* In case the autocommand move the cursor to a position that that
	     * not exist in curbuf. */
	    check_cursor();
	}
    }
}