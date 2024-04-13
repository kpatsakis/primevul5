trigger_cursorhold(void)
{
    int		state;

    if (!did_cursorhold
	    && has_cursorhold()
	    && !Recording
	    && typebuf.tb_len == 0
#ifdef FEAT_INS_EXPAND
	    && !ins_compl_active()
#endif
	    )
    {
	state = get_real_state();
	if (state == NORMAL_BUSY || (state & INSERT) != 0)
	    return TRUE;
    }
    return FALSE;
}