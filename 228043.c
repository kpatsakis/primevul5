openscript(
    char_u	*name,
    int		directly)	// when TRUE execute directly
{
    if (curscript + 1 == NSCRIPT)
    {
	emsg(_(e_scripts_nested_too_deep));
	return;
    }

    // Disallow sourcing a file in the sandbox, the commands would be executed
    // later, possibly outside of the sandbox.
    if (check_secure())
	return;

#ifdef FEAT_EVAL
    if (ignore_script)
	// Not reading from script, also don't open one.  Warning message?
	return;
#endif

    if (scriptin[curscript] != NULL)	// already reading script
	++curscript;
				// use NameBuff for expanded name
    expand_env(name, NameBuff, MAXPATHL);
    if ((scriptin[curscript] = mch_fopen((char *)NameBuff, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str), name);
	if (curscript)
	    --curscript;
	return;
    }
    if (save_typebuf() == FAIL)
	return;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    if (directly)
    {
	oparg_T	oa;
	int	oldcurscript;
	int	save_State = State;
	int	save_restart_edit = restart_edit;
	int	save_insertmode = p_im;
	int	save_finish_op = finish_op;
	int	save_msg_scroll = msg_scroll;

	State = NORMAL;
	msg_scroll = FALSE;	// no msg scrolling in Normal mode
	restart_edit = 0;	// don't go to Insert mode
	p_im = FALSE;		// don't use 'insertmode'
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	do
	{
	    update_topline_cursor();	// update cursor position and topline
	    normal_cmd(&oa, FALSE);	// execute one command
	    (void)vpeekc();		// check for end of file
	}
	while (scriptin[oldcurscript] != NULL);

	State = save_State;
	msg_scroll = save_msg_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}