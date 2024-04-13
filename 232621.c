buf_check_timestamp(
    buf_T	*buf,
    int		focus UNUSED)	/* called for GUI focus event */
{
    stat_T	st;
    int		stat_res;
    int		retval = 0;
    char_u	*path;
    char_u	*tbuf;
    char	*mesg = NULL;
    char	*mesg2 = "";
    int		helpmesg = FALSE;
    int		reload = FALSE;
    char	*reason;
#if defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    int		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    int		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    int		save_mouse_correct = need_mouse_correct;
#endif
#ifdef FEAT_AUTOCMD
    static int	busy = FALSE;
    int		n;
    char_u	*s;
    bufref_T	bufref;

    set_bufref(&bufref, buf);
#endif

    /* If there is no file name, the buffer is not loaded, 'buftype' is
     * set, we are in the middle of a save or being called recursively: ignore
     * this buffer. */
    if (buf->b_ffname == NULL
	    || buf->b_ml.ml_mfp == NULL
	    || *buf->b_p_bt != NUL
	    || buf->b_saving
#ifdef FEAT_AUTOCMD
	    || busy
#endif
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != NULL
#endif
	    )
	return 0;

    if (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((char *)buf->b_ffname, &st)) < 0
		|| time_differs((long)st.st_mtime, buf->b_mtime)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (int)st.st_mode != buf->b_orig_mode
#else
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	retval = 1;

	/* set b_mtime to stop further warnings (e.g., when executing
	 * FileChangedShell autocmd) */
	if (stat_res < 0)
	{
	    buf->b_mtime = 0;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	else
	    buf_store_time(buf, &st, buf->b_ffname);

	/* Don't do anything for a directory.  Might contain the file
	 * explorer. */
	if (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	else if ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = TRUE;
	else
	{
	    if (stat_res < 0)
		reason = "deleted";
	    else if (bufIsChanged(buf))
		reason = "conflict";
	    else if (orig_size != buf->b_orig_size || buf_contents_changed(buf))
		reason = "changed";
	    else if (orig_mode != buf->b_orig_mode)
		reason = "mode";
	    else
		reason = "time";

#ifdef FEAT_AUTOCMD
	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, -1);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", -1);
# endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    if (n)
	    {
		if (!bufref_valid(&bufref))
		    EMSG(_("E246: FileChangedShell autocommand deleted buffer"));
# ifdef FEAT_EVAL
		s = get_vim_var_str(VV_FCS_CHOICE);
		if (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = TRUE;
		else if (STRCMP(s, "ask") == 0)
		    n = FALSE;
		else
# endif
		    return 2;
	    }
	    if (!n)
#endif
	    {
		if (*reason == 'd')
		    mesg = _("E211: File \"%s\" no longer available");
		else
		{
		    helpmesg = TRUE;
#if defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    /*
		     * Check if the file contents really changed to avoid
		     * giving a warning when only the timestamp was set (e.g.,
		     * checked out of CVS).  Always warn when the buffer was
		     * changed.
		     */
		    if (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    else if (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    else if (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    else
			/* Only timestamp changed, store it to avoid a warning
			 * in check_mtime() later. */
			buf->b_mtime_read = buf->b_mtime;
		}
	    }
	}

    }
    else if ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#if defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    if (mesg != NULL)
    {
	path = home_replace_save(buf, buf->b_fname);
	if (path != NULL)
	{
	    if (!helpmesg)
		mesg2 = "";
	    tbuf = alloc((unsigned)(STRLEN(path) + STRLEN(mesg)
							+ STRLEN(mesg2) + 2));
	    sprintf((char *)tbuf, mesg, path);
#ifdef FEAT_EVAL
	    /* Set warningmsg here, before the unimportant and output-specific
	     * mesg2 has been appended. */
	    set_vim_var_string(VV_WARNINGMSG, tbuf, -1);
#endif
#if defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	    if (can_reload)
	    {
		if (*mesg2 != NUL)
		{
		    STRCAT(tbuf, "\n");
		    STRCAT(tbuf, mesg2);
		}
		if (do_dialog(VIM_WARNING, (char_u *)_("Warning"), tbuf,
			  (char_u *)_("&OK\n&Load File"), 1, NULL, TRUE) == 2)
		    reload = TRUE;
	    }
	    else
#endif
	    if (State > NORMAL_BUSY || (State & CMDLINE) || already_warned)
	    {
		if (*mesg2 != NUL)
		{
		    STRCAT(tbuf, "; ");
		    STRCAT(tbuf, mesg2);
		}
		EMSG(tbuf);
		retval = 2;
	    }
	    else
	    {
# ifdef FEAT_AUTOCMD
		if (!autocmd_busy)
# endif
		{
		    msg_start();
		    msg_puts_attr(tbuf, HL_ATTR(HLF_E) + MSG_HIST);
		    if (*mesg2 != NUL)
			msg_puts_attr((char_u *)mesg2,
						   HL_ATTR(HLF_W) + MSG_HIST);
		    msg_clr_eos();
		    (void)msg_end();
		    if (emsg_silent == 0)
		    {
			out_flush();
# ifdef FEAT_GUI
			if (!focus)
# endif
			    /* give the user some time to think about it */
			    ui_delay(1000L, TRUE);

			/* don't redraw and erase the message */
			redraw_cmdline = FALSE;
		    }
		}
		already_warned = TRUE;
	    }

	    vim_free(path);
	    vim_free(tbuf);
	}
    }

    if (reload)
    {
	/* Reload the buffer. */
	buf_reload(buf, orig_mode);
#ifdef FEAT_PERSISTENT_UNDO
	if (buf->b_p_udf && buf->b_ffname != NULL)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    /* Any existing undo file is unusable, write it now. */
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(NULL, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

#ifdef FEAT_AUTOCMD
    /* Trigger FileChangedShell when the file was changed in any way. */
    if (bufref_valid(&bufref) && retval != 0)
	(void)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#endif
#ifdef FEAT_GUI
    /* restore this in case an autocommand has set it; it would break
     * 'mousefocus' */
    need_mouse_correct = save_mouse_correct;
#endif

    return retval;
}