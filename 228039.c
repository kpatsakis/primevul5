gotchars(char_u *chars, int len)
{
    char_u		*s = chars;
    int			i;
    static char_u	buf[4];
    static int		buflen = 0;
    int			todo = len;

    while (todo--)
    {
	buf[buflen++] = *s++;

	// When receiving a special key sequence, store it until we have all
	// the bytes and we can decide what to do with it.
	if (buflen == 1 && buf[0] == K_SPECIAL)
	    continue;
	if (buflen == 2)
	    continue;
	if (buflen == 3 && buf[1] == KS_EXTRA
		       && (buf[2] == KE_FOCUSGAINED || buf[2] == KE_FOCUSLOST))
	{
	    // Drop K_FOCUSGAINED and K_FOCUSLOST, they are not useful in a
	    // recording.
	    buflen = 0;
	    continue;
	}

	// Handle one byte at a time; no translation to be done.
	for (i = 0; i < buflen; ++i)
	    updatescript(buf[i]);

	if (reg_recording != 0)
	{
	    buf[buflen] = NUL;
	    add_buff(&recordbuff, buf, (long)buflen);
	    // remember how many chars were last recorded
	    last_recorded_len += buflen;
	}
	buflen = 0;
    }
    may_sync_undo();

#ifdef FEAT_EVAL
    // output "debug mode" message next time in debug mode
    debug_did_msg = FALSE;
#endif

    // Since characters have been typed, consider the following to be in
    // another mapping.  Search string will be kept in history.
    ++maptick;
}