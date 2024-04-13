check_timestamps(
    int		focus)		/* called for GUI focus event */
{
    buf_T	*buf;
    int		didit = 0;
    int		n;

    /* Don't check timestamps while system() or another low-level function may
     * cause us to lose and gain focus. */
    if (no_check_timestamps > 0)
	return FALSE;

    /* Avoid doing a check twice.  The OK/Reload dialog can cause a focus
     * event and we would keep on checking if the file is steadily growing.
     * Do check again after typing something. */
    if (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	return FALSE;
    }

    if (!stuff_empty() || global_busy || !typebuf_typed()
#ifdef FEAT_AUTOCMD
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0
#endif
					)
	need_check_timestamps = TRUE;		/* check later */
    else
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    /* Only check buffers in a window. */
	    if (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		if (didit < n)
		    didit = n;
		if (n > 0 && !bufref_valid(&bufref))
		{
		    /* Autocommands have removed the buffer, start at the
		     * first one again. */
		    buf = firstbuf;
		    continue;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	if (need_wait_return && didit == 2)
	{
	    /* make sure msg isn't overwritten */
	    msg_puts((char_u *)"\n");
	    out_flush();
	}
    }
    return didit;
}