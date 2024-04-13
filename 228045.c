inchar(
    char_u	*buf,
    int		maxlen,
    long	wait_time)	    // milliseconds
{
    int		len = 0;	    // init for GCC
    int		retesc = FALSE;	    // return ESC with gotint
    int		script_char;
    int		tb_change_cnt = typebuf.tb_change_cnt;

    if (wait_time == -1L || wait_time > 100L)  // flush output before waiting
    {
	cursor_on();
	out_flush_cursor(FALSE, FALSE);
#if defined(FEAT_GUI) && defined(FEAT_MOUSESHAPE)
	if (gui.in_use && postponed_mouseshape)
	    update_mouseshape(-1);
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    if (State != HITRETURN)
    {
	did_outofmem_msg = FALSE;   // display out of memory message (again)
	did_swapwrite_msg = FALSE;  // display swap file write error again
    }
    undo_off = FALSE;		    // restart undo now

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    while (scriptin[curscript] != NULL && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {
#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	if (got_int || (script_char = getc(scriptin[curscript])) < 0)
	{
	    // Reached EOF.
	    // Careful: closescript() frees typebuf.tb_buf[] and buf[] may
	    // point inside typebuf.tb_buf[].  Don't use buf[] after this!
	    closescript();
	    /*
	     * When reading script file is interrupted, return an ESC to get
	     * back to normal mode.
	     * Otherwise return -1, because typebuf.tb_buf[] has changed.
	     */
	    if (got_int)
		retesc = TRUE;
	    else
		return -1;
	}
	else
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    if (script_char < 0)	// did not get a character from script
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	if (got_int)
	{
#define DUM_LEN MAXMAPLEN * 3 + 3
	    char_u	dum[DUM_LEN + 1];

	    for (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		if (len == 0 || (len == 1 && dum[0] == 3))
		    break;
	    }
	    return retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user and not just peeking.
	 */
	if (wait_time == -1L || wait_time > 10L)
	    out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    // If the typebuf was changed further down, it is like nothing was added by
    // this call.
    if (typebuf_changed(tb_change_cnt))
	return 0;

    // Note the change in the typeahead buffer, this matters for when
    // vgetorpeek() is called recursively, e.g. using getchar(1) in a timer
    // function.
    if (len > 0 && ++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    return fix_input_buffer(buf, len);
}