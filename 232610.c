ex_doautoall(exarg_T *eap)
{
    int		retval;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    int		call_do_modelines = check_nomodeline(&arg);
    int		did_aucmd;

    /*
     * This is a bit tricky: For some commands curwin->w_buffer needs to be
     * equal to curbuf, but for some buffers there may not be a window.
     * So we change the buffer for the current window for a moment.  This
     * gives problems when the autocommands make changes to the list of
     * buffers or windows...
     */
    FOR_ALL_BUFFERS(buf)
    {
	if (buf->b_ml.ml_mfp != NULL)
	{
	    /* find a window for this buffer and save some values */
	    aucmd_prepbuf(&aco, buf);
	    set_bufref(&bufref, buf);

	    /* execute the autocommands for this buffer */
	    retval = do_doautocmd(arg, FALSE, &did_aucmd);

	    if (call_do_modelines && did_aucmd)
	    {
		/* Execute the modeline settings, but don't set window-local
		 * options if we are using the current window for another
		 * buffer. */
		do_modelines(curwin == aucmd_win ? OPT_NOWIN : 0);
	    }

	    /* restore the current window */
	    aucmd_restbuf(&aco);

	    /* stop if there is some error or buffer was deleted */
	    if (retval == FAIL || !bufref_valid(&bufref))
		break;
	}
    }

    check_cursor();	    /* just in case lines got deleted */
}