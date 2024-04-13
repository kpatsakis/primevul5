ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    if (!check_restricted())
    {
	if (!eap->forceit)
	    autowrite_all();
	apply_autocmds(EVENT_VIMSUSPEND, NULL, NULL, FALSE, NULL);
	windgoto((int)Rows - 1, 0);
	out_char('\n');
	out_flush();
	stoptermcap();
	out_flush();		// needed for SUN to restore xterm buffer
#ifdef FEAT_TITLE
	mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
#endif
	ui_suspend();		// call machine specific function
#ifdef FEAT_TITLE
	maketitle();
	resettitle();		// force updating the title
#endif
	starttermcap();
	scroll_start();		// scroll screen before redrawing
	redraw_later_clear();
	shell_resized();	// may have resized window
	apply_autocmds(EVENT_VIMRESUME, NULL, NULL, FALSE, NULL);
    }
}