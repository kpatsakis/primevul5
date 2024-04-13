parse_queued_messages(void)
{
    int	    old_curwin_id;
    int	    old_curbuf_fnum;
    int	    i;
    int	    save_may_garbage_collect = may_garbage_collect;
    static int entered = 0;
    int	    was_safe = get_was_safe_state();

    // Do not handle messages while redrawing, because it may cause buffers to
    // change or be wiped while they are being redrawn.
    // Also bail out when parsing messages was explicitly disabled.
    if (updating_screen || dont_parse_messages)
	return;

    // If memory allocation fails during startup we'll exit but curbuf or
    // curwin could be NULL.
    if (curbuf == NULL || curwin == NULL)
       return;

    old_curbuf_fnum = curbuf->b_fnum;
    old_curwin_id = curwin->w_id;

    ++entered;

    // may_garbage_collect is set in main_loop() to do garbage collection when
    // blocking to wait on a character.  We don't want that while parsing
    // messages, a callback may invoke vgetc() while lists and dicts are in use
    // in the call stack.
    may_garbage_collect = FALSE;

    // Loop when a job ended, but don't keep looping forever.
    for (i = 0; i < MAX_REPEAT_PARSE; ++i)
    {
	// For Win32 mch_breakcheck() does not check for input, do it here.
# if (defined(MSWIN) || defined(__HAIKU__)) && defined(FEAT_JOB_CHANNEL)
	channel_handle_events(FALSE);
# endif

# ifdef FEAT_NETBEANS_INTG
	// Process the queued netbeans messages.
	netbeans_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Write any buffer lines still to be written.
	channel_write_any_lines();

	// Process the messages queued on channels.
	channel_parse_messages();
# endif
# if defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	// Process the queued clientserver messages.
	server_parse_messages();
# endif
# ifdef FEAT_JOB_CHANNEL
	// Check if any jobs have ended.  If so, repeat the above to handle
	// changes, e.g. stdin may have been closed.
	if (job_check_ended())
	    continue;
# endif
# ifdef FEAT_TERMINAL
	free_unused_terminals();
# endif
# ifdef FEAT_SOUND_CANBERRA
	if (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
#ifdef SIGUSR1
	if (got_sigusr1)
	{
	    apply_autocmds(EVENT_SIGUSR1, NULL, NULL, FALSE, curbuf);
	    got_sigusr1 = FALSE;
	}
#endif
	break;
    }

    // When not nested we'll go back to waiting for a typed character.  If it
    // was safe before then this triggers a SafeStateAgain autocommand event.
    if (entered == 1 && was_safe)
	may_trigger_safestateagain();

    may_garbage_collect = save_may_garbage_collect;

    // If the current window or buffer changed we need to bail out of the
    // waiting loop.  E.g. when a job exit callback closes the terminal window.
    if (curwin->w_id != old_curwin_id || curbuf->b_fnum != old_curbuf_fnum)
	ins_char_typebuf(K_IGNORE, 0);

    --entered;
}