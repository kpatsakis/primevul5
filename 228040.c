vgetorpeek(int advance)
{
    int		c, c1;
    int		timedout = FALSE;	// waited for more than 1 second
					// for mapping to complete
    int		mapdepth = 0;		// check for recursive mapping
    int		mode_deleted = FALSE;   // set when mode has been deleted
#ifdef FEAT_CMDL_INFO
    int		new_wcol, new_wrow;
#endif
#ifdef FEAT_GUI
    int		shape_changed = FALSE;  // adjusted cursor shape
#endif
    int		n;
    int		old_wcol, old_wrow;
    int		wait_tb_len;

    /*
     * This function doesn't work very well when called recursively.  This may
     * happen though, because of:
     * 1. The call to add_to_showcmd().	char_avail() is then used to check if
     * there is a character available, which calls this function.  In that
     * case we must return NUL, to indicate no character is available.
     * 2. A GUI callback function writes to the screen, causing a
     * wait_return().
     * Using ":normal" can also do this, but it saves the typeahead buffer,
     * thus it should be OK.  But don't get a key from the user then.
     */
    if (vgetc_busy > 0 && ex_normal_busy == 0)
	return NUL;

    ++vgetc_busy;

    if (advance)
	KeyStuffed = FALSE;

    init_typebuf();
    start_stuff();
    if (advance && typebuf.tb_maplen == 0)
	reg_executing = 0;
    do
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	if (typeahead_char != 0)
	{
	    c = typeahead_char;
	    if (advance)
		typeahead_char = 0;
	}
	else
	    c = read_readbuffers(advance);
	if (c != NUL && !got_int)
	{
	    if (advance)
	    {
		// KeyTyped = FALSE;  When the command that stuffed something
		// was typed, behave like the stuffed command was typed.
		// needed for CTRL-W CTRL-] to open a fold, for example.
		KeyStuffed = TRUE;
	    }
	    if (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	// no abbreviations now
	}
	else
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    for (;;)
	    {
		long	wait_time;
		int	keylen = 0;
#ifdef FEAT_CMDL_INFO
		int	showcmd_idx;
#endif
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		if (typebuf.tb_maplen)
		    line_breakcheck();
		else
		    ui_breakcheck();		// check for CTRL-C
		if (got_int)
		{
		    // flush all input
		    c = inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 0L);

		    /*
		     * If inchar() returns TRUE (script file was active) or we
		     * are inside a mapping, get out of Insert mode.
		     * Otherwise we behave like having gotten a CTRL-C.
		     * As a result typing CTRL-C in insert mode will
		     * really insert a CTRL-C.
		     */
		    if ((c || typebuf.tb_maplen)
					      && (State & (INSERT + CMDLINE)))
			c = ESC;
		    else
			c = Ctrl_C;
		    flush_buffers(FLUSH_INPUT);	// flush all typeahead

		    if (advance)
		    {
			// Also record this character, it might be needed to
			// get out of Insert mode.
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    break;
		}
		else if (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mapping in "typebuf".
		     */
		    map_result_T result = handle_mapping(
						&keylen, &timedout, &mapdepth);

		    if (result == map_result_retry)
			// try mapping again
			continue;
		    if (result == map_result_fail)
		    {
			// failed, use the outer loop
			c = -1;
			break;
		    }
		    if (result == map_result_get)
		    {
/*
 * get a character: 2. from the typeahead buffer
 */
			c = typebuf.tb_buf[typebuf.tb_off];
			if (advance)	// remove chars from tb_buf
			{
			    cmd_silent = (typebuf.tb_silent > 0);
			    if (typebuf.tb_maplen > 0)
				KeyTyped = FALSE;
			    else
			    {
				KeyTyped = TRUE;
				// write char to script file(s)
				gotchars(typebuf.tb_buf
						 + typebuf.tb_off, 1);
			    }
			    KeyNoremap = typebuf.tb_noremap[
						      typebuf.tb_off];
			    del_typebuf(1, 0);
			}
			break;  // got character, break the for loop
		    }

		    // not enough characters, get more
		}

/*
 * get a character: 3. from the user - handle <Esc> in Insert mode
 */
		/*
		 * Special case: if we get an <ESC> in insert mode and there
		 * are no more characters at once, we pretend to go out of
		 * insert mode.  This prevents the one second delay after
		 * typing an <ESC>.  If we get something after all, we may
		 * have to redisplay the mode. That the cursor is in the wrong
		 * place does not matter.
		 */
		c = 0;
#ifdef FEAT_CMDL_INFO
		new_wcol = curwin->w_wcol;
		new_wrow = curwin->w_wrow;
#endif
		if (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & INSERT)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_KEY && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0, vcol;
		    char_u	*ptr;

		    if (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    // may show a different cursor shape
		    if (gui.in_use && State != NORMAL && !cmd_silent)
		    {
			int	    save_State;

			save_State = State;
			State = NORMAL;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_changed = TRUE;
		    }
#endif
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    // move cursor left, if possible
		    if (curwin->w_cursor.col != 0)
		    {
			if (curwin->w_wcol > 0)
			{
			    if (did_ai)
			    {
				/*
				 * We are expecting to truncate the trailing
				 * white-space, so find the last non-white
				 * character -- webb
				 */
				col = vcol = curwin->w_wcol = 0;
				ptr = ml_get_curline();
				while (col < curwin->w_cursor.col)
				{
				    if (!VIM_ISWHITE(ptr[col]))
					curwin->w_wcol = vcol;
				    vcol += lbr_chartabsize(ptr, ptr + col,
							       vcol);
				    if (has_mbyte)
					col += (*mb_ptr2len)(ptr + col);
				    else
					++col;
				}
				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	// no correction needed
			    }
			    else
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			else if (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			if (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    // Correct when the cursor is on the right halve
			    // of a double-wide character.
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    if ((*mb_ptr2cells)(ptr + col) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    out_flush();
#ifdef FEAT_CMDL_INFO
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
#endif
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		if (c < 0)
		    continue;	// end of input script reached

		// Allow mapping for just typed characters. When we get here c
		// is the number of extra bytes and typebuf.tb_len is 1.
		for (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		// buffer full, don't map
		if (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    continue;
		}

		if (ex_normal_busy > 0)
		{
#ifdef FEAT_CMDWIN
		    static int tc = 0;
#endif

		    // No typeahead left and inside ":normal".  Must return
		    // something to avoid getting stuck.  When an incomplete
		    // mapping is present, behave like it timed out.
		    if (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			continue;
		    }

		    // When 'insertmode' is set, ESC just beeps in Insert
		    // mode.  Use CTRL-L to make edit() return.
		    // For the command line only CTRL-C always breaks it.
		    // For the cmdline window: Alternate between ESC and
		    // CTRL-C: ESC for most situations and CTRL-C to close the
		    // cmdline window.
		    if (p_im && (State & INSERT))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    else if (terminal_is_active())
			c = K_CANCEL;
#endif
		    else if ((State & CMDLINE)
#ifdef FEAT_CMDWIN
			    || (cmdwin_type > 0 && tc == ESC)
#endif
			    )
			c = Ctrl_C;
		    else
			c = ESC;
#ifdef FEAT_CMDWIN
		    tc = c;
#endif
		    // return from main_loop()
		    if (pending_exmode_active)
			exmode_active = EXMODE_NORMAL;

		    // no chars to block abbreviation for
		    typebuf.tb_no_abbr_cnt = 0;

		    break;
		}

/*
 * get a character: 3. from the user - update display
 */
		// In insert mode a screen update is skipped when characters
		// are still available.  But when those available characters
		// are part of a mapping, and we are going to do a blocking
		// wait here.  Need to update the screen to display the
		// changed text so far. Also for when 'lazyredraw' is set and
		// redrawing was postponed because there was something in the
		// input buffer (e.g., termresponse).
		if (((State & INSERT) != 0 || p_lz) && (State & CMDLINE) == 0
			  && advance && must_redraw != 0 && !need_wait_return)
		{
		    update_screen(0);
		    setcursor(); // put cursor back where it belongs
		}

		/*
		 * If we have a partial match (and are going to wait for more
		 * input from the user), show the partially matched characters
		 * to the user with showcmd.
		 */
#ifdef FEAT_CMDL_INFO
		showcmd_idx = 0;
#endif
		c1 = 0;
		if (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    if (((State & (NORMAL | INSERT)) || State == LANGMAP)
			    && State != HITRETURN)
		    {
			// this looks nice when typing a dead character map
			if (State & INSERT
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
			{
			    edit_putchar(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			    setcursor(); // put cursor back where it belongs
			    c1 = 1;
			}
#ifdef FEAT_CMDL_INFO
			// need to use the col and row from above here
			old_wcol = curwin->w_wcol;
			old_wrow = curwin->w_wrow;
			curwin->w_wcol = new_wcol;
			curwin->w_wrow = new_wrow;
			push_showcmd();
			if (typebuf.tb_len > SHOWCMD_COLS)
			    showcmd_idx = typebuf.tb_len - SHOWCMD_COLS;
			while (showcmd_idx < typebuf.tb_len)
			    (void)add_to_showcmd(
			       typebuf.tb_buf[typebuf.tb_off + showcmd_idx++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
#endif
		    }

		    // this looks nice when typing a dead character map
		    if ((State & CMDLINE)
#if defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_star == 0
#endif
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
		    {
			putcmdline(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			c1 = 1;
		    }
		}

/*
 * get a character: 3. from the user - get it
 */
		if (typebuf.tb_len == 0)
		    // timedout may have been set while waiting for a mapping
		    // that has a <Nop> RHS.
		    timedout = FALSE;

		if (advance)
		{
		    if (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_KEY)))
			// blocking wait
			wait_time = -1L;
		    else if (keylen == KEYLEN_PART_KEY && p_ttm >= 0)
			wait_time = p_ttm;
		    else
			wait_time = p_tm;
		}
		else
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

#ifdef FEAT_CMDL_INFO
		if (showcmd_idx != 0)
		    pop_showcmd();
#endif
		if (c1 == 1)
		{
		    if (State & INSERT)
			edit_unputchar();
		    if (State & CMDLINE)
			unputcmdline();
		    else
			setcursor();	// put cursor back where it belongs
		}

		if (c < 0)
		    continue;		// end of input script reached
		if (c == NUL)		// no character available
		{
		    if (!advance)
			break;
		    if (wait_tb_len > 0)	// timed out
		    {
			timedout = TRUE;
			continue;
		    }
		}
		else
		{	    // allow mapping for just typed characters
		    while (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_INPUT_METHOD
		    // Get IM status right after getting keys, not after the
		    // timeout for a mapping (focus may be lost by then).
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    // for (;;)
	}	// if (!character from stuffbuf)

	// if advance is FALSE don't loop on NULs
    } while ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    if (advance && p_smd && msg_silent == 0 && (State & INSERT))
    {
	if (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    // delete mode later
	    else
		unshowmode(FALSE);
	}
	else if (c != ESC && mode_deleted)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    // show mode later
	    else
		showmode();
	}
    }
#ifdef FEAT_GUI
    // may unshow different cursor shape
    if (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    if (timedout && c == ESC)
    {
	char_u nop_buf[3];

	// When recording there will be no timeout.  Add a <Nop> after the ESC
	// to avoid that it forms a key code with following characters.
	nop_buf[0] = K_SPECIAL;
	nop_buf[1] = KS_EXTRA;
	nop_buf[2] = KE_NOP;
	gotchars(nop_buf, 3);
    }

    --vgetc_busy;

    return c;
}