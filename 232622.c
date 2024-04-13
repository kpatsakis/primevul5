filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    int		attr)
{
    int		msg_scroll_save;

    if (msg_silent != 0)
	return;
    msg_add_fname(buf, name);	    /* put file name in IObuff with quotes */
    /* If it's extremely long, truncate it. */
    if (STRLEN(IObuff) > IOSIZE - 80)
	IObuff[IOSIZE - 80] = NUL;
    STRCAT(IObuff, s);
    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    msg_scroll_save = msg_scroll;
    if (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	msg_scroll = FALSE;
    if (!msg_scroll)	/* wait a bit when overwriting an error msg */
	check_for_delay(FALSE);
    msg_start();
    msg_scroll = msg_scroll_save;
    msg_scrolled_ign = TRUE;
    /* may truncate the message to avoid a hit-return prompt */
    msg_outtrans_attr(msg_may_trunc(FALSE, IObuff), attr);
    msg_clr_eos();
    out_flush();
    msg_scrolled_ign = FALSE;
}