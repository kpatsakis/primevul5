aucmd_prepbuf(
    aco_save_T	*aco,		/* structure to save values in */
    buf_T	*buf)		/* new curbuf */
{
    win_T	*win;
    int		save_ea;
#ifdef FEAT_AUTOCHDIR
    int		save_acd;
#endif

    /* Find a window that is for the new buffer */
    if (buf == curbuf)		/* be quick when buf is curbuf */
	win = curwin;
    else
	FOR_ALL_WINDOWS(win)
	    if (win->w_buffer == buf)
		break;

    /* Allocate "aucmd_win" when needed.  If this fails (out of memory) fall
     * back to using the current window. */
    if (win == NULL && aucmd_win == NULL)
    {
	win_alloc_aucmd_win();
	if (aucmd_win == NULL)
	    win = curwin;
    }
    if (win == NULL && aucmd_win_used)
	/* Strange recursive autocommand, fall back to using the current
	 * window.  Expect a few side effects... */
	win = curwin;

    aco->save_curwin = curwin;
    aco->save_curbuf = curbuf;
    if (win != NULL)
    {
	/* There is a window for "buf" in the current tab page, make it the
	 * curwin.  This is preferred, it has the least side effects (esp. if
	 * "buf" is curbuf). */
	aco->use_aucmd_win = FALSE;
	curwin = win;
    }
    else
    {
	/* There is no window for "buf", use "aucmd_win".  To minimize the side
	 * effects, insert it in the current tab page.
	 * Anything related to a window (e.g., setting folds) may have
	 * unexpected results. */
	aco->use_aucmd_win = TRUE;
	aucmd_win_used = TRUE;
	aucmd_win->w_buffer = buf;
	aucmd_win->w_s = &buf->b_s;
	++buf->b_nwindows;
	win_init_empty(aucmd_win); /* set cursor and topline to safe values */

	/* Make sure w_localdir and globaldir are NULL to avoid a chdir() in
	 * win_enter_ext(). */
	vim_free(aucmd_win->w_localdir);
	aucmd_win->w_localdir = NULL;
	aco->globaldir = globaldir;
	globaldir = NULL;


	/* Split the current window, put the aucmd_win in the upper half.
	 * We don't want the BufEnter or WinEnter autocommands. */
	block_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#ifdef FEAT_AUTOCHDIR
	/* Prevent chdir() call in win_enter_ext(), through do_autochdir(). */
	save_acd = p_acd;
	p_acd = FALSE;
#endif

	(void)win_split_ins(0, WSP_TOP, aucmd_win, 0);
	(void)win_comp_pos();   /* recompute window positions */
	p_ea = save_ea;
#ifdef FEAT_AUTOCHDIR
	p_acd = save_acd;
#endif
	unblock_autocmds();
	curwin = aucmd_win;
    }
    curbuf = buf;
    aco->new_curwin = curwin;
    set_bufref(&aco->new_curbuf, curbuf);
}