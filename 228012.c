saveRedobuff(save_redo_T *save_redo)
{
    char_u	*s;

    save_redo->sr_redobuff = redobuff;
    redobuff.bh_first.b_next = NULL;
    save_redo->sr_old_redobuff = old_redobuff;
    old_redobuff.bh_first.b_next = NULL;

    // Make a copy, so that ":normal ." in a function works.
    s = get_buffcont(&save_redo->sr_redobuff, FALSE);
    if (s != NULL)
    {
	add_buff(&redobuff, s, -1L);
	vim_free(s);
    }
}