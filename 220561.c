do_exmode(
    int		improved)	    // TRUE for "improved Ex" mode
{
    int		save_msg_scroll;
    int		prev_msg_row;
    linenr_T	prev_line;
    varnumber_T	changedtick;

    if (improved)
	exmode_active = EXMODE_VIM;
    else
	exmode_active = EXMODE_NORMAL;
    State = NORMAL;
    trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    if (global_busy)
	return;

    save_msg_scroll = msg_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    msg(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    while (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	if (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    break;
	}
	msg_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_msg_row = msg_row;
	prev_line = curwin->w_cursor.lnum;
	if (improved)
	{
	    cmdline_row = msg_row;
	    do_cmdline(NULL, getexline, NULL, 0);
	}
	else
	    do_cmdline(NULL, getexmodeline, NULL, DOCMD_NOWAIT);
	lines_left = Rows - 1;

	if ((prev_line != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    if (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_emptybuf));
	    else
	    {
		if (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    msg_row = prev_msg_row;
		    if (prev_msg_row == Rows - 1)
			msg_row--;
		}
		msg_col = 0;
		print_line_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		msg_clr_eos();
	    }
	}
	else if (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    if (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_emptybuf));
	    else
		emsg(_("E501: At end-of-file"));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    --RedrawingDisabled;
    --no_wait_return;
    update_screen(CLEAR);
    need_wait_return = FALSE;
    msg_scroll = save_msg_scroll;
}